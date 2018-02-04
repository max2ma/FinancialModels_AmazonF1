/*
======================================================
 Copyright 2016 Liang Ma

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
======================================================

*----------------------------------------------------------------------------
*
* Author:   Liang Ma (liang-ma@polito.it)
*
* Top function is defined here with interface specified as axi4.
* It creates an object of heston and launches the simulation.
*
*----------------------------------------------------------------------------
*/
#include "hestonEuroBarrier.h"

extern "C"
void hestonEuroBarrier(data_t *pCall, data_t *pPut,   // call price and put price
		data_t expect,				// theta
		data_t kappa,				// kappa
		data_t variance,			// xi
		data_t correlation,			// rho
		data_t timeT,				// time period of options
		data_t freeRate,			// interest rate of the riskless asset
		data_t volatility,			// volatility of the risky asset
		data_t initPrice,			// stock price at time 0
		data_t strikePrice,			// strike price
		data_t upB,				// up barrier
		data_t lowB				// low barrier
		)
{
#pragma HLS INTERFACE m_axi port=pCall bundle=gmem
#pragma HLS INTERFACE s_axilite port=pCall bundle=control
#pragma HLS INTERFACE m_axi port=pPut bundle=gmem
#pragma HLS INTERFACE s_axilite port=pPut bundle=control
#pragma HLS INTERFACE s_axilite port=correlation bundle=gmem
#pragma HLS INTERFACE s_axilite port=correlation bundle=control
#pragma HLS INTERFACE s_axilite port=variance bundle=gmem
#pragma HLS INTERFACE s_axilite port=variance bundle=control
#pragma HLS INTERFACE s_axilite port=kappa bundle=gmem
#pragma HLS INTERFACE s_axilite port=kappa bundle=control
#pragma HLS INTERFACE s_axilite port=expect bundle=gmem
#pragma HLS INTERFACE s_axilite port=expect bundle=control
#pragma HLS INTERFACE s_axilite port=timeT bundle=gmem
#pragma HLS INTERFACE s_axilite port=timeT bundle=control
#pragma HLS INTERFACE s_axilite port=freeRate bundle=gmem
#pragma HLS INTERFACE s_axilite port=freeRate bundle=control
#pragma HLS INTERFACE s_axilite port=volatility bundle=gmem
#pragma HLS INTERFACE s_axilite port=volatility bundle=control
#pragma HLS INTERFACE s_axilite port=initPrice bundle=gmem
#pragma HLS INTERFACE s_axilite port=initPrice bundle=control
#pragma HLS INTERFACE s_axilite port=strikePrice bundle=gmem
#pragma HLS INTERFACE s_axilite port=strikePrice bundle=control
#pragma HLS INTERFACE s_axilite port=lowB bundle=gmem
#pragma HLS INTERFACE s_axilite port=lowB bundle=control
#pragma HLS INTERFACE s_axilite port=upB bundle=gmem
#pragma HLS INTERFACE s_axilite port=upB bundle=control
#pragma HLS INTERFACE s_axilite port=return bundle=control

	volData vol(expect,kappa,variance,volatility,correlation);
	stockData sd(timeT,freeRate,volatility,initPrice,strikePrice);
	barrierData bD(upB,lowB);
	heston bs(sd,vol,bD);
	data_t call,put;
	bs.simulation(&call,&put);

	*pCall=call;
	*pPut=put;
	return;
}




const int heston::NUM_RNGS=32;
const int heston::NUM_SIMGROUPS=512;
const int heston::NUM_STEPS=256;
const int heston::NUM_SIMS=512;

heston::heston(stockData data,volData vol,barrierData bData)
	:data(data),vol(vol),bData(bData)
{
}
void heston::simulation(data_t* pCall, data_t *pPut)
{

	RNG mt_rng[NUM_RNGS];
#pragma HLS ARRAY_PARTITION variable=mt_rng complete dim=1

	uint seeds[NUM_RNGS];
#pragma HLS ARRAY_PARTITION variable=seeds complete dim=1

	loop_seed:for(int i=0;i<NUM_RNGS;i++)
	{
#pragma HLS UNROLL
		seeds[i]=i;
	}
	RNG::init_array(mt_rng,seeds,NUM_RNGS);
	return sampleSIM(mt_rng,pCall,pPut);

}
void heston::sampleSIM(RNG* mt_rng, data_t* call,data_t* put)
{
	const data_t Dt=data.timeT/NUM_STEPS,
			ratio1=expf(-data.freeRate*data.timeT),
			ratio2=sqrtf(fmaxf(1.0f-vol.correlation*vol.correlation,0.0f)),
			ratio3=Dt*data.freeRate,
			ratio4=vol.kappa*vol.expect*Dt,
			volInit =fmaxf(vol.initValue,0.0f)*Dt;

	data_t fCall=0.0f,fPut=0.0f;
	data_t sCall[NUM_RNGS],sPut[NUM_RNGS];
#pragma HLS ARRAY_PARTITION variable=sCall complete dim=1
#pragma HLS ARRAY_PARTITION variable=sPut complete dim=1

	data_t stockPrice[NUM_RNGS][NUM_SIMGROUPS];
#pragma HLS ARRAY_PARTITION variable=stockPrice complete dim=1

	data_t vols[NUM_RNGS][NUM_SIMGROUPS],pVols[NUM_RNGS][NUM_SIMGROUPS];

#pragma HLS ARRAY_PARTITION variable=vols complete dim=1
#pragma HLS ARRAY_PARTITION variable=pVols complete dim=1

	data_t num1[NUM_RNGS][NUM_SIMGROUPS],num2[NUM_RNGS][NUM_SIMGROUPS];
#pragma HLS ARRAY_PARTITION variable=num2 complete dim=1
#pragma HLS ARRAY_PARTITION variable=num1 complete dim=1

	data_t bBarrier[NUM_RNGS][NUM_SIMGROUPS];
#pragma HLS ARRAY_PARTITION variable=bBarrier complete dim=1

	for(int i =0;i<NUM_RNGS;i++)
	{
#pragma HLS UNROLL
		sCall[i]=0.0f;
		sPut[i]=0.0f;
	}
	loop_init:for(int s=0;s<NUM_SIMGROUPS;s++)
	{
		for(int i =0;i<NUM_RNGS;i++)
		{
	#pragma HLS UNROLL
			stockPrice[i][s]=data.initPrice;
			vols[i][s]=vol.initValue;
			pVols[i][s]=volInit;
			bBarrier[i][s]=true;
		}
	}
	loop_main:for(int j=0;j<NUM_SIMS;j++)
	{
		loop_path:for(int path=0;path<NUM_STEPS;path++)
		{
			loop_share:for(int s=0;s<NUM_SIMGROUPS;s++)
			{
		#pragma HLS PIPELINE
				loop_parallel:for(uint i=0;i<NUM_RNGS;i++)
				{
#pragma HLS UNROLL
					if(!bBarrier[i][s])
						continue;
					mt_rng[i].BOX_MULLER(&num1[i][s],&num2[i][s],pVols[i][s]);

					stockPrice[i][s]*=expf(ratio3-pVols[i][s]*0.5f+num1[i][s]*vol.correlation+num2[i][s]*ratio2);
					if(stockPrice[i][s]<bData.downBarrier || stockPrice[i][s]>bData.upBarrier)
					{
						bBarrier[i][s]=false;
					}
					vols[i][s]+=ratio4-vol.kappa*pVols[i][s]+vol.variance*num1[i][s];
					pVols[i][s]=fmaxf(vols[i][s],0.0f)*Dt;

				}
			}
		}
		loop_sum:for(int s=0;s<NUM_SIMGROUPS;s++)
		{
			loop_sum_R:for(int i =0;i<NUM_RNGS;i++)
			{
#pragma HLS UNROLL
				vols[i][s]=vol.initValue;
				pVols[i][s]=volInit;
				if(bBarrier[i][s])
				{
					float payoff = stockPrice[i][s]-data.strikePrice;
					if(stockPrice[i][s]>data.strikePrice)
					{
						sCall[i]+= payoff;
					}
					else
					{
						sPut[i]-=payoff;
					}
				}
				stockPrice[i][s]=data.initPrice;
				bBarrier[i][s]=true;
			}
		}
	}
	loop_final_sum:for(int i =0;i<NUM_RNGS;i++)
	{
#pragma HLS UNROLL
		fCall+=sCall[i];
		fPut+=sPut[i];
	}
	*call= ratio1*fCall/NUM_RNGS/NUM_SIMS/NUM_SIMGROUPS;
	*put= ratio1*fPut/NUM_RNGS/NUM_SIMS/NUM_SIMGROUPS;
}

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
*
* Author:   Liang Ma (liang-ma@polito.it)
*
* Top function is defined here with interface specified as axi4.
* It creates an object of blackScholes and launches the simulation.
*
*----------------------------------------------------------------------------
*/
#include "RNG.h"
#include "blackScholes.h"
#include "defTypes.h"
#include "stockData.h"

template<typename DATA_T, typename BS>
void launchSimulation(DATA_T &pCall, DATA_T &pPut, RNG<DATA_T> &rng0,RNG<DATA_T> &rng1, BS &bs, int nums, int sims, int steps)
{
#pragma HLS INLINE off
#pragma HLS DATAFLOW
	hls::stream<DATA_T> sRNG0;
	hls::stream<DATA_T> sRNG1;
#pragma HLS STREAM variable=sRNG0 depth=2 dim=1
#pragma HLS STREAM variable=sRNG1 depth=2 dim=1
	for(int i = 0 ; i < nums/4;i++){
#pragma HLS PIPELINE
		DATA_T r0, r1, r2, r3;
		rng0.BOX_MULLER(&r0, &r1, 0, 1);
		rng1.BOX_MULLER(&r2, &r3, 0, 1);
		sRNG0.write(r0);
		sRNG0.write(r2);
		sRNG1.write(r1);
		sRNG1.write(r3);
	}
	bs.simulation(sRNG0,sRNG1, sims, pCall, pPut);
}

	extern "C"
void blackEuro(data_t *pCall, data_t *pPut,   // call price and put price
		data_t timeT,				// time period of options
		data_t freeRate,			// interest rate of the riskless asset
		data_t volatility,			// volatility of the risky asset
		data_t initPrice,			// stock price at time 0
		data_t strikePrice,// strike price
		data_t steps,
		data_t sims,
		data_t g_id)			
{
#pragma HLS INTERFACE m_axi port=pCall bundle=gmem
#pragma HLS INTERFACE s_axilite port=pCall bundle=control
#pragma HLS INTERFACE m_axi port=pPut bundle=gmem
#pragma HLS INTERFACE s_axilite port=pPut bundle=control
#pragma HLS INTERFACE s_axilite port=timeT bundle=control
#pragma HLS INTERFACE s_axilite port=freeRate bundle=control
#pragma HLS INTERFACE s_axilite port=volatility bundle=control
#pragma HLS INTERFACE s_axilite port=initPrice bundle=control
#pragma HLS INTERFACE s_axilite port=strikePrice bundle=control
#pragma HLS INTERFACE s_axilite port=steps bundle=control
#pragma HLS INTERFACE s_axilite port=sims bundle=control
#pragma HLS INTERFACE s_axilite port=g_id bundle=control
#pragma HLS INTERFACE s_axilite port=return bundle=control

#pragma HLS ALLOCATION instances=mul limit=1 operation
#pragma HLS ALLOCATION instances=fmul limit=1 operation
#pragma HLS ALLOCATION instances=fexp limit=1 operation
#pragma HLS ALLOCATION instances=fdiv limit=1 operation
	static const int SIMS_PER_GROUP =32;
	data_t call, put;
	stockData<data_t> sd(timeT,freeRate,volatility,initPrice,strikePrice);
	blackScholes<SIMS_PER_GROUP,data_t> bs(sd, steps);
	RNG<data_t> rng0(g_id);
	RNG<data_t> rng1(~((int)g_id+1));
	launchSimulation(call, put, rng0,rng1, bs, (int)sims*(int)steps, sims, steps);
	pCall[(int)g_id] = call/sims;
	pPut[(int)g_id] = put/sims;
}

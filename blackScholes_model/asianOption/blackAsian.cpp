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
#include "stockData.h"
#include "blackScholes.h"
#include "AsianOption.h"
#include "launchSim.h"


extern "C"
void blackAsian(float *pCall, float *pPut,   // call price and put price
		float timeT,				// time period of options
		float freeRate,			// interest rate of the riskless asset
		float volatility,			// volatility of the risky asset
		float initPrice,			// stock price at time 0
		float strikePrice,// strike price
		float steps,
		float sims,
		float g_id)			
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
	float call, put;
	stockData<float> sd(timeT,freeRate,volatility,initPrice,strikePrice);
	blackScholes<SIMS_PER_GROUP,AsianOptionStatus<float>, float> bs(sd, steps);
	AsianOptionStatus<float>::dt = 1.0f/steps;
	RNG<float> rng0(g_id);
	RNG<float> rng1(~((int)g_id+1));
	launchSimulation(call, put, rng0,rng1, bs, int(sims)*int(steps), sims, steps);
	pCall[(int)g_id] = call/sims;
	pPut[(int)g_id] = put/sims;
}

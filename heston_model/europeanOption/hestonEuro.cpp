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
----------------------------------------------------------------------------
*
* Author:   Liang Ma (liang-ma@polito.it)
*
*----------------------------------------------------------------------------
*/
#include "hestonEuro.h"
#include "launchSim.h"
#include "hestonModel.h"
extern "C"
void hestonEuro(float *pCall, float *pPut,   // call price and put price
		float expect,				// theta
		float kappa,				// kappa
		float variance,			// xi
		float correlation,			// rho
		float timeT,				// time period of options
		float freeRate,			// interest rate of the riskless asset
		float volatility,			// volatility of the risky asset
		float initPrice,			// stock price at time 0
		float strikePrice,			// strike price
		int sims,
		int steps,
		int g_id
		){
#pragma HLS INTERFACE m_axi port=pCall bundle=gmem
#pragma HLS INTERFACE s_axilite port=pCall bundle=control
#pragma HLS INTERFACE m_axi port=pPut bundle=gmem
#pragma HLS INTERFACE s_axilite port=pPut bundle=control
#pragma HLS INTERFACE s_axilite port=correlation bundle=control
#pragma HLS INTERFACE s_axilite port=variance bundle=control
#pragma HLS INTERFACE s_axilite port=kappa bundle=control
#pragma HLS INTERFACE s_axilite port=expect bundle=control
#pragma HLS INTERFACE s_axilite port=timeT bundle=control
#pragma HLS INTERFACE s_axilite port=freeRate bundle=control
#pragma HLS INTERFACE s_axilite port=volatility bundle=control
#pragma HLS INTERFACE s_axilite port=initPrice bundle=control
#pragma HLS INTERFACE s_axilite port=strikePrice bundle=control
#pragma HLS INTERFACE s_axilite port=sims bundle=control
#pragma HLS INTERFACE s_axilite port=steps bundle=control
#pragma HLS INTERFACE s_axilite port=g_id bundle=control
#pragma HLS INTERFACE s_axilite port=return bundle=control

#pragma HLS ALLOCATION instances=mul limit=1 operation
#pragma HLS ALLOCATION instances=div limit=1 operation
#pragma HLS ALLOCATION instances=fadd limit=1 operation
#pragma HLS ALLOCATION instances=fmul limit=1 operation
#pragma HLS ALLOCATION instances=fdiv limit=1 operation
#pragma HLS ALLOCATION instances=fsqrt limit=1 operation
#pragma HLS ALLOCATION instances=uitof limit=1 operation
	static const int NUM_SIMS=64;

	const int each_sims = sims >> 1;
	volData<float> vol(expect,kappa,variance,volatility,correlation);
	stockData<float> sd(timeT,freeRate,volatility,initPrice,strikePrice);
	float call0, put0;
	heston<NUM_SIMS,EuropeanOptionStatus<float>, float> hs0(sd,vol, steps);
	float call1, put1;
	heston<NUM_SIMS,EuropeanOptionStatus<float>, float> hs1(sd,vol, steps);
	{
#pragma HLS FUNCTION_EXTRACT
#pragma HLS DATAFLOW
		launchSimulation(call0, put0, g_id, hs0, each_sims*steps<<1, each_sims);
		launchSimulation(call1, put1, g_id<<1, hs1, each_sims*steps<<1, each_sims);

	}
	pCall[g_id]=(
			call0
			+call1
			)/sims;

	pPut[g_id]=(
			put0
			+put1
			)/sims;
	return;
}


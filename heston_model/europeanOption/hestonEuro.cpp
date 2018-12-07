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
	static const int NUM_SIMS=64;

	static const int MULTI = 14;
	const int each_sims = sims/MULTI;
	const int last_sims = sims - each_sims * (MULTI - 1);
	volData<float> vol(expect,kappa,variance,volatility,correlation);
	stockData<float> sd(timeT,freeRate,volatility,initPrice,strikePrice);
	float call0, put0;
	heston<NUM_SIMS,EuropeanOptionStatus<float>, float> hs0(sd,vol, steps);
	float call1, put1;
	heston<NUM_SIMS,EuropeanOptionStatus<float>, float> hs1(sd,vol, steps);
	float call2, put2;
	heston<NUM_SIMS,EuropeanOptionStatus<float>, float> hs2(sd,vol, steps);
	float call3, put3;
	heston<NUM_SIMS,EuropeanOptionStatus<float>, float> hs3(sd,vol, steps);
	float call4, put4;
	heston<NUM_SIMS,EuropeanOptionStatus<float>, float> hs4(sd,vol, steps);
	float call5, put5;
	heston<NUM_SIMS,EuropeanOptionStatus<float>, float> hs5(sd,vol, steps);
	float call6, put6;
	heston<NUM_SIMS,EuropeanOptionStatus<float>, float> hs6(sd,vol, steps);
	float call7, put7;
	heston<NUM_SIMS,EuropeanOptionStatus<float>, float> hs7(sd,vol, steps);
	float call8, put8;
	heston<NUM_SIMS,EuropeanOptionStatus<float>, float> hs8(sd,vol, steps);
	float call9, put9;
	heston<NUM_SIMS,EuropeanOptionStatus<float>, float> hs9(sd,vol, steps);
	float call10, put10;
	heston<NUM_SIMS,EuropeanOptionStatus<float>, float> hs10(sd,vol, steps);
	float call11, put11;
	heston<NUM_SIMS,EuropeanOptionStatus<float>, float> hs11(sd,vol, steps);
	float call12, put12;
	heston<NUM_SIMS,EuropeanOptionStatus<float>, float> hs12(sd,vol, steps);
	float call13, put13;
	heston<NUM_SIMS,EuropeanOptionStatus<float>, float> hs13(sd,vol, steps);
	{
#pragma HLS FUNCTION_EXTRACT
#pragma HLS DATAFLOW
		launchSimulation(call0, put0, g_id+0, hs0, 2*each_sims*steps, each_sims);
		launchSimulation(call1, put1, g_id+1, hs1, 2*each_sims*steps, each_sims);
		launchSimulation(call2, put2, g_id+2, hs2, 2*each_sims*steps, each_sims);
		launchSimulation(call3, put3, g_id+3, hs3, 2*each_sims*steps, each_sims);
		launchSimulation(call4, put4, g_id+4, hs4, 2*each_sims*steps, each_sims);
		launchSimulation(call5, put5, g_id+5, hs5, 2*each_sims*steps, each_sims);
		launchSimulation(call6, put6, g_id+6, hs6, 2*each_sims*steps, each_sims);
		launchSimulation(call7, put7, g_id+7, hs7, 2*each_sims*steps, each_sims);
		launchSimulation(call8, put8, g_id+8, hs8, 2*each_sims*steps, each_sims);
		launchSimulation(call9, put9, g_id+9, hs9, 2*each_sims*steps, each_sims);
		launchSimulation(call10, put10, g_id+10, hs10, 2*each_sims*steps, each_sims);
		launchSimulation(call11, put11, g_id+11, hs11, 2*each_sims*steps, each_sims);
		launchSimulation(call12, put12, g_id+12, hs12, 2*each_sims*steps, each_sims);
		launchSimulation(call13, put13, g_id+13, hs13, 2*last_sims*steps, last_sims);
	}
	pCall[g_id]=(
			call0
			+call1
			+call2
			+call3
			+call4
			+call5
			+call6
			+call7
			+call8
			+call9
			+call10
			+call11
			+call12
			+call13
			)/sims;

	pPut[g_id]=(
			put0
			+put1
			+put2
			+put3
			+put4
			+put5
			+put6
			+put7
			+put8
			+put9
			+put10
			+put11
			+put12
			+put13
			)/sims;
	return;
}


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

template<int NUM_STEPS, int SIMS_PER_GROUP, typename DATA_T>
void launchSimulation(DATA_T &pCall, DATA_T &pPut, RNG<DATA_T> &rng, blackScholes<NUM_STEPS, SIMS_PER_GROUP,DATA_T> &bs, int sims)
{
#pragma HLS INLINE off
#pragma HLS DATAFLOW
		hls::stream<data_t> sRNG;
#pragma HLS stream variable=sRNG depth=2
		rng.generateStream(sims*NUM_STEPS, sRNG, 0, 1);
		bs.simulation(sRNG, sims, pCall, pPut);
}

extern "C"
void blackEuro(data_t *pCall, data_t *pPut,   // call price and put price
		data_t timeT,				// time period of options
		data_t freeRate,			// interest rate of the riskless asset
		data_t volatility,			// volatility of the risky asset
		data_t initPrice,			// stock price at time 0
		data_t strikePrice,// strike price
		data_t sims,
		data_t seed)			
{
#pragma HLS INTERFACE m_axi port=pCall bundle=gmem
#pragma HLS INTERFACE s_axilite port=pCall bundle=control
#pragma HLS INTERFACE m_axi port=pPut bundle=gmem
#pragma HLS INTERFACE s_axilite port=pPut bundle=control
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
#pragma HLS INTERFACE s_axilite port=sims bundle=gmem
#pragma HLS INTERFACE s_axilite port=sims bundle=control
#pragma HLS INTERFACE s_axilite port=seed bundle=gmem
#pragma HLS INTERFACE s_axilite port=seed bundle=control
#pragma HLS INTERFACE s_axilite port=return bundle=control

	static const int STEPS = 64, SIMS_PER_GROUP =32;
	data_t call, put;
	stockData<data_t> sd(timeT,freeRate,volatility,initPrice,strikePrice);
	blackScholes<STEPS, SIMS_PER_GROUP,data_t> bs(sd);
	RNG<data_t> rng(seed);
	launchSimulation<STEPS, SIMS_PER_GROUP, data_t>(call, put, rng, bs, sims);
	*pCall = call/sims;
	*pPut = put/sims;
}

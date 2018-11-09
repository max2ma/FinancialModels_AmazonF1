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
* This class is called "blackScholes" which implements the BS model.
*
* simulation() is launched by top function.
* It defines and initializes some objects of RNG and then launch the simulation.
*
* sampleSIM performs the entire simulation and passes the results to top function.
*
*----------------------------------------------------------------------------
*/
#ifndef __BLACKSCHOLES__
#define __BLACKSCHOLES__
#include "hls_stream.h"
#include "stockData.h"
using namespace std;

template<int NUM_SIMS, typename DATA_T>
class blackScholes
{
	const stockData<DATA_T> data;
	const int NUM_STEPS;
	const DATA_T Dt, Vol, SqrtV, Interest;
	public:
	blackScholes(stockData<DATA_T>&data, int num_steps):data(data),
	NUM_STEPS(num_steps),
	Dt(data.timeT / (DATA_T)NUM_STEPS),
	Vol(expf((data.freeRate- 0.5f * data.volatility * data.volatility)*Dt)),
	SqrtV(data.volatility * sqrtf(Dt)),
	Interest(expf(-data.freeRate * data.timeT)){
	}
	void simulation(hls::stream<DATA_T>& s_RNG0,hls::stream<DATA_T>& s_RNG1, int sims,  DATA_T &pCall, DATA_T &pPut)
	{
		DATA_T sumCall=0.0f,sumPut=0.0f;

		for(int k=0;k<sims/NUM_SIMS;k++) {

			DATA_T stockPrice[NUM_SIMS];
#pragma HLS ARRAY_PARTITION variable=stockPrice cyclic factor=2 dim=1
			for(int j=0;j<NUM_SIMS;j++)
#pragma HLS UNROLL
				stockPrice[j] = data.price;

			for(int s=0; s <NUM_STEPS;s++){
				for(int j=0;j<NUM_SIMS/2;j++) {
#pragma HLS PIPELINE
					DATA_T r0 = s_RNG0.read();
					DATA_T r1 = s_RNG1.read();
					update(stockPrice[j*2], r0);
					update(stockPrice[j*2+1], r1);
				}
			}

			for(int j=0;j<NUM_SIMS;j++) {
#pragma HLS UNROLL
				sumCall+=executeCall(stockPrice[j]);
				sumPut+=executePut(stockPrice[j]);
			}
		}
		pCall=Interest * sumCall;
		pPut =Interest * sumPut;
	}
	void update(DATA_T& price, const DATA_T &r){
#pragma HLS INLINE
		price *= Vol * expf(r *SqrtV);
	}
	DATA_T executeCall(DATA_T& price){
#pragma HLS INLINE
		if(price > data.strikePrice){
			return (price - data.strikePrice);
		}
		else
			return 0;
	}
	DATA_T executePut(DATA_T& price){
	#pragma HLS INLINE
		if(price < data.strikePrice){
			return (data.strikePrice - price);
		}
		else
			return 0;
	}
};

#endif

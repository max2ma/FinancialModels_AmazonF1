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
 * This class is called "heston" which implements the Heston model.
 *
 * simulation() is launched by top function.
 * It defines and initializes some objects of RNG and then launch the simulation.
 *
 * sampleSIM performs the entire simulation and passes the results to top function.
 *
 *----------------------------------------------------------------------------
 */
#ifndef __HESTON_H__
#define __HESTON_H__

#include "barrierData.h"
#include "RNG.h"

template<typename DATA_T>
class EuropeanBarrierOptionStatus{
	public:
		EuropeanBarrierOptionStatus(DATA_T p = 0, DATA_T v=0){
			valid = true;
			stockPrice = p;
			volatility = v;
		}
		void init(DATA_T p, DATA_T v){
			valid = true;
			stockPrice = p;
			volatility = v;
		}

		void update(){
			if(valid == false)
				return;
			if(stockPrice<bData.downBarrier || stockPrice>bData.upBarrier){
				valid=false;
#ifdef REPORT
				giveup++;
#endif
			}
		}
		bool valid;
		DATA_T stockPrice;
		DATA_T volatility;
		static barrierData<DATA_T> bData;
#ifdef REPORT
		static int giveup;
#endif
};

#ifdef REPORT
template<typename T>
int EuropeanBarrierOptionStatus<T>::giveup = 0;
#endif

template<typename T>
barrierData<T> EuropeanBarrierOptionStatus<T>::bData(0, 0);

#endif

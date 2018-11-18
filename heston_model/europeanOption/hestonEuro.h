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
#pragma once
#include "RNG.h"

template<typename DATA_T>
class EuropeanOptionStatus{
	public:
		EuropeanOptionStatus(DATA_T p = 0, DATA_T v=0){
			stockPrice = p;
			volatility = v;
			valid = true;
		}
		void init(DATA_T p, DATA_T v){
			stockPrice = p;
			volatility = v;
			valid = true;
		}

		void update(){
		}
		DATA_T stockPrice;
		DATA_T volatility;
		bool valid;
};

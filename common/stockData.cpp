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
*----------------------------------------------------------------------------
*/

#include "stockData.h"

template<typename DATA_T>
inline stockData<DATA_T>::stockData(DATA_T timeT, DATA_T freeRate, DATA_T volatility,
		DATA_T price,DATA_T strikePrice)
{
	this->freeRate=freeRate;
	this->price=price;
	this->timeT=timeT;
	this->volatility=volatility;
	this->strikePrice=strikePrice;
}

template<typename DATA_T>
inline stockData<DATA_T>::stockData(const stockData<DATA_T>& data)
{
	this->freeRate=data.freeRate;
	this->price=data.price;
	this->timeT=data.timeT;
	this->volatility=data.volatility;
	this->strikePrice=data.strikePrice;
}
#if 0
template<typename DATA_T>
void stockData<DATA_T>::print()const
{
	std::cout<<"timeT:"<<timeT<<' '
			<<"freeRate:"<<freeRate<<' '
			<<"volatility:"<<volatility<<' '
			<<"price:"<<price<<' '
			<<"strikePrice:"<<strikePrice
			<<std::endl;
}
#endif

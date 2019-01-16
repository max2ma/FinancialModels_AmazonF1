#pragma once
#include "RNG.h"

template<typename DATA_T>
class AsianOptionStatus{
	public:
		AsianOptionStatus(DATA_T p = 0){
			executePrice=0;
			stockPrice = p;
			valid = true;
		}
		void init(DATA_T p){
			executePrice=0;
			stockPrice = p;
			valid = true;
		}

		void update(){
			executePrice+=stockPrice * dt;
		}

		DATA_T price(){
			return executePrice;
		}
		
		DATA_T stockPrice;
		DATA_T executePrice;
		bool valid;
		static DATA_T dt;
};

template<typename DATA_T>
 DATA_T AsianOptionStatus<DATA_T>::dt = 0;

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
 * NUM_RNGS is the number of RNG objects implemented
 *  NUM_OPS_GROUPS 	is the number simulation groups (it must be an even number)
 * NUM_STEPS	is the number of time steps/partitions, which is 1 for the European option.
 *
 * init_array(), a static member function of RNG is defined here due to the dimension of the array.
 *
 * The algorithm in sampleSIM() is optimized from the mathematical and FPGA
 * implementation points of view for the European option.
 *
 *----------------------------------------------------------------------------
 */
#include "blackScholes.h"

	template<int NUM_OPS_GROUPS, int NUM_STEPS, int NUM_SIMS, typename OPTION, typename DATA_T>
blackScholes<NUM_OPS_GROUPS, NUM_STEPS, NUM_SIMS, OPTION, DATA_T>::blackScholes(OPTION* data):option(data)
{
}

	template<int NUM_OPS_GROUPS, int NUM_STEPS, int NUM_SIMS, typename OPTION, typename DATA_T>
void blackScholes<NUM_OPS_GROUPS, NUM_STEPS, NUM_SIMS, OPTION, DATA_T>::simulation(hls::stream<DATA_T>& s_RNG, DATA_T * pCall, DATA_T *pPut)
{
	DATA_T sumCall=0.0f,sumPut=0.0f;

	for(int k=0;k<NUM_SIMS;k++) {
		OPTION options[NUM_OPS_GROUPS];
		for(int j=0;j<NUM_OPS_GROUPS;j++)
#pragma HLS PIPELINE
			options[j].init(*option);
		for(int s=0; s <NUM_STEPS;s++){
			for(int j=0;j<NUM_OPS_GROUPS;j+=2) {
#pragma HLS PIPELINE
				DATA_T r0 = s_RNG.read();
				DATA_T r1 = s_RNG.read();
				options[j].update(r0, NUM_STEPS);
				options[j+1].update(r1, NUM_STEPS);
			}
		}

		for(int j=0;j<NUM_OPS_GROUPS;j++) {
#pragma HLS UNROLL
			sumCall+=options[j].executeCall();
			sumPut+=options[j].executePut();
		}
	}
	*pCall=sumCall/NUM_OPS_GROUPS/NUM_SIMS;
	*pPut =sumPut/NUM_OPS_GROUPS/NUM_SIMS;
}


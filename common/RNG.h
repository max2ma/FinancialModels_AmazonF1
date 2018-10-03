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
 * Here declares the class RNG which is short for Random Number Generator.
 * It implements 32bits Mersenne-Twist algorithm and Box-Muller transformation.
 *
 * RNG_N is the total size of the states
 * mt_e/mt_o are two arrays that stores the states.
 * extract_number produces two uniformly i.i.d. numbers
 * init()/init_array() are two functions for state initialization.
 *
 *----------------------------------------------------------------------------
 */

#pragma once
#include <cmath>
#include "hls_stream.h"
template<typename DATA_T>
class RNG
{

	static const int RNG_N = 624;
	static const int RNG_H = 312;

	static const int RNG_MH = 198;
	static const int RNG_MHI = 199;

	static const int RNG_F = 0x6C078965;
	static const int RNG_W = 32;
	static const int RNG_M = 397;

	static const int RNG_R = 31;

	static const int RNG_U = 11;
	static const int RNG_A = 0x9908B0DF;
	static const int RNG_D = 0xFFFFFFFF;
	static const int RNG_S = 7;
	static const int RNG_B = 0x9D2C5680;
	static const int RNG_T = 15;
	static const int RNG_C = 0xEFC60000;
	static const int RNG_L = 18;

	static const int lower_mask = 0x7FFFFFFF;
	static const int upper_mask = 0x80000000;

	public:
	typedef unsigned int uint;
	int index;
	uint seed;
	uint mt_e[RNG_H],mt_o[RNG_H];

	RNG();
	RNG(uint);
	void init(uint);
	void extract_number(uint*,uint*);
	void BOX_MULLER(DATA_T*, DATA_T*,DATA_T, DATA_T);
	int increase(int);
	int & operator ++();
	void generateStream(int, hls::stream<DATA_T>&, DATA_T, DATA_T);

	static void init_array(RNG*, uint*,const int);
};


/*======================================================
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
 * In the definition of Box_Muller(), sin/cos are used instead of sinf/cosf
 * due to bugs in the synthesizer (C/RTL co-simulation is failing).
 *
 *
 *----------------------------------------------------------------------------
 */

#include "RNG.h"
#include "ap_fixed.h"
template<typename DATA_T>
RNG<DATA_T>::RNG(){
}

template<typename DATA_T>
RNG<DATA_T>::RNG(uint seed){
	this->index = 0;
	this->seed=seed;
	uint tmp=seed;

	for (int i = 0; i < RNG_H; i++)
	{
#pragma HLS PIPELINE off
		mt_e[i]=tmp;
		tmp= RNG_F*(tmp^ (tmp >> (RNG_W - 2))) + (i>>1) +1;
		mt_o[i]=tmp;
		tmp= RNG_F*(tmp^ (tmp >> (RNG_W - 2))) + (i>>1) +2;
	}
}
template<typename DATA_T>
void RNG<DATA_T>::init(uint seed){
#pragma HLS INLINE
	this->index = 0;
	this->seed=seed;
	uint tmp=seed;

	for (int i = 0; i < RNG_H; i++)
	{
		mt_e[i]=tmp;
		tmp= RNG_F*(tmp^ (tmp >> (RNG_W - 2))) + i*2+1;
		mt_o[i]=tmp;
		tmp= RNG_F*(tmp^ (tmp >> (RNG_W - 2))) + i*2+2;
	}
}
template<typename DATA_T>
void RNG<DATA_T>::init_array(RNG* rng, uint* seed, const uint size)
{
	uint tmp[size];
#pragma HLS ARRAY_PARTITION variable=tmp complete dim=1

	for(int i=0;i<size;i++)
	{
#pragma HLS UNROLL
		rng[i].index = 0;
		rng[i].seed=seed[i];
		tmp[i]=seed[i];
	}


	for (int i = 0; i < RNG_H; i++)
	{
		for(int k=0;k<size;k++)
		{
#pragma HLS UNROLL
			rng[k].mt_e[i]=tmp[k];
			tmp[k]= RNG_F*(tmp[k]^ (tmp[k] >> (RNG_W - 2))) + i*2+1;
			rng[k].mt_o[i]=tmp[k];
			tmp[k]= RNG_F*(tmp[k]^ (tmp[k] >> (RNG_W - 2))) + i*2+2;
		}
	}
}


template<typename DATA_T>
void RNG<DATA_T>::extract_number(uint *num1, uint *num2)
{
#pragma HLS DEPENDENCE variable=mt_e array inter RAW false
#pragma HLS DEPENDENCE variable=mt_o array inter RAW false
#pragma HLS INLINE
	uint id1=increase(1), idm=increase(RNG_MH), idm1=increase(RNG_MHI);

	uint x = this->seed,x1=this->mt_o[this->index],x2=this->mt_e[id1],
			 xm=this->mt_o[idm],xm1=this->mt_e[idm1];

	x = (x & upper_mask)+(x1 & lower_mask);
	uint xp = x >> 1;
	if ((x & 0x01) != 0)
		xp ^= RNG_A;
	x = xm ^ xp;

	uint y = x;
	y ^= ((y >> RNG_U)&RNG_D);
	y ^= ((y << RNG_S)&RNG_B);
	y ^= ((y << RNG_T)&RNG_C);
	y ^= (y >> RNG_L);
	*num1 = y;
	mt_e[this->index]=x;

	x1 =( x1 & upper_mask) + (x2 & lower_mask);
	uint xt = x1 >> 1;
	if ((x1 &0x01) != 0)
		xt ^= RNG_A;
	x1 = xm1 ^ xt;

	uint y1 = x1;
	y1 ^= ((y1 >> RNG_U)&RNG_D);
	y1 ^= ((y1 << RNG_S)&RNG_B);
	y1 ^= ((y1 << RNG_T)&RNG_C);
	y1 ^= (y1 >> RNG_L);
	*num2 = y1;
	mt_o[this->index]=x1;

	this->index=id1;
	this->seed=x2;
}


template<typename DATA_T>
unsigned int RNG<DATA_T>::increase(uint k)
{
	uint tmp= this->index+k;
	return (tmp>=RNG_H)? tmp-RNG_H:tmp;
}

template<typename DATA_T>
void RNG<DATA_T>::BOX_MULLER(DATA_T *data1, DATA_T *data2,DATA_T ave, DATA_T deviation)
{
#pragma HLS INLINE
	static const DATA_T _2PI= 2*3.14159265358979323846f;
//	static const DATA_T MINI_RNG = 2.328306e-10;

#pragma HLS ALLOCATION instances=fmul limit=1 operation
	uint num1,num2;
	DATA_T tp,tmp1,tmp2;
	extract_number(&num1,&num2);
#if 1	
	ap_ufixed<32,0> f_tmp1, f_tmp2;
	f_tmp1(31, 0)=num1;
	f_tmp2(31, 0)=num2;
	tmp1 = f_tmp1.to_float();
	tmp2 = f_tmp2.to_float();
#else	
	tmp1=num1*MINI_RNG;
	tmp2=num2*MINI_RNG;
#endif
#ifdef __DOUBLE_PRECISION__
	tp=sqrt(fmax(-2*log(tmp1),0)*deviation);
	*data1=cos(_2PI*tmp2)*tp+ave;
	*data2=sin(_2PI*tmp2)*tp+ave;
#else
	tp=sqrtf(fmaxf(-2.0f*logf(tmp1),0.0f)*deviation);
	*data1=cosf(_2PI*tmp2)*tp+ave;
	*data2=sinf(_2PI*tmp2)*tp+ave;
#endif
}

template<typename DATA_T>
void RNG<DATA_T>::generateStream(uint NUM, hls::stream<DATA_T>&sRNG, DATA_T mean, DATA_T std){
	for(int i = 0 ; i < NUM/2;i++){
#pragma HLS PIPELINE
		DATA_T r0, r1;
		this->BOX_MULLER(&r0, &r1, mean, std);
		sRNG.write(r0);
		sRNG.write(r1);
	}
}

template class RNG<float>;
#ifdef __DOUBLE_PRECISION__
template class RNG<double>;
#endif

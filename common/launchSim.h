
#pragma once
#include "hls_stream.h"
#include "RNG.h"
	template<typename DATA_T>
void prng(RNG<DATA_T> &rng0,RNG<DATA_T> &rng1, hls::stream<DATA_T> &sRNG0,hls::stream<DATA_T> &sRNG1, int nums)
{
	for(int i = 0 ; i < nums/4;i++){
#pragma HLS PIPELINE
		DATA_T r0, r1, r2, r3;
		rng0.BOX_MULLER(&r0, &r1, 0, 1);
		rng1.BOX_MULLER(&r2, &r3, 0, 1);
		sRNG0.write(r0);
		sRNG0.write(r2);
		sRNG1.write(r1);
		sRNG1.write(r3);
	}
}
	template<typename DATA_T, typename Model>
void launchSimulation(DATA_T &pCall, DATA_T &pPut, RNG<DATA_T> &rng0,RNG<DATA_T> &rng1, Model &m, int numR, int sims, int steps)
{
#pragma HLS DATAFLOW
	hls::stream<DATA_T> sRNG0;
	hls::stream<DATA_T> sRNG1;
#pragma HLS STREAM variable=sRNG0 depth=2 dim=1
#pragma HLS STREAM variable=sRNG1 depth=2 dim=1
	prng(rng0, rng1, sRNG0, sRNG1, numR);
	m.simulation(sRNG0,sRNG1, sims, pCall, pPut);
}

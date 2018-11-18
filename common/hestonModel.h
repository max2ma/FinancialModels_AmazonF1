#pragma once
#include "stockData.h"
#include "volatilityData.h"
#include "RNG.h"
template<int NUM_SIMS, typename OptionStatus, typename DATA_T>
class heston
{
	const stockData<DATA_T> data;
	const volData<DATA_T> vol;
	const int NUM_STEPS;

	public:
	heston(stockData<DATA_T> &data,volData<DATA_T> &vol, const int steps)
		:data(data),vol(vol),NUM_STEPS(steps){
		}

	void simulation(hls::stream<DATA_T>& s_RNG0,hls::stream<DATA_T>& s_RNG1, int sims, DATA_T &pCall, DATA_T &pPut){
		DATA_T call = 0, put = 0;
		hls::stream<DATA_T> prices;
#pragma HLS STREAM variable=prices depth=NUM_SIMS dim=1
#pragma HLS DATAFLOW
		path_sim(s_RNG0, s_RNG1, prices, sims);
		sum(prices, call, put, sims);
		pCall= call;
		pPut = put;
	}



	void path_sim(hls::stream<DATA_T>& s_RNG0,hls::stream<DATA_T>& s_RNG1, hls::stream<DATA_T>&prices, int sims){
		OptionStatus stocks[NUM_SIMS];

		for(int s=0;s<NUM_SIMS;s++){
#pragma HLS PIPELINE
			stocks[s].init(data.price, vol.initValue);
		}

		for(int j=0;j<sims/NUM_SIMS;j++)
		{
			for(int path=0;path<NUM_STEPS;path++)
			{
				for(int s=0;s<NUM_SIMS;s++)
				{
#pragma HLS PIPELINE
					DATA_T num0 = s_RNG0.read();
					DATA_T num1 = s_RNG1.read();
					if(stocks[s].valid)
						update(stocks[s], num0, num1);
				}
			}
			for(int s=0;s<NUM_SIMS;s++)
			{
#pragma HLS PIPELINE
				DATA_T price = stocks[s].valid? stocks[s].stockPrice : data.strikePrice;
				prices.write(price);
				stocks[s].init(data.price, vol.initValue);
			}
		}
	}
	void update(OptionStatus &stocks, DATA_T num0, DATA_T num1){
#pragma HLS INLINE
		const DATA_T Dt=data.timeT/NUM_STEPS,
					rho1=sqrtf(fmaxf(1.0f-vol.correlation*vol.correlation,0.0f)),
					rdt=Dt*data.freeRate,
					ktdt=vol.kappa*vol.expect*Dt;

		DATA_T sqrtV = sqrtf(stocks.volatility* Dt);
		stocks.stockPrice*=1 + rdt + sqrtV * (vol.correlation * num0 + rho1 * num1);
//		stocks.stockPrice*=expf(rdt -0.5*stocks.volatility*Dt + sqrtV * (vol.correlation * num0 + rho1 * num1));
		stocks.update();
		stocks.volatility+=ktdt-vol.kappa*stocks.volatility*Dt+vol.variance*num0*sqrtV;
		stocks.volatility=fmaxf(stocks.volatility,0.0f);
	}
	void sum(hls::stream<DATA_T> &prices, DATA_T &call, DATA_T&put, int sims){
		for(int i=0;i<sims;i++) {
#pragma HLS PIPELINE
			DATA_T price = prices.read();
			execute(price, call, put);
		}
	}
	void execute(DATA_T &price, DATA_T &call, DATA_T &put){
		float payoff = price-data.strikePrice;
		if(payoff >0){
			call+= payoff;
		}
		else{
			put-=payoff;
		}
	}
};


/*----------------------------------------------------------------------------
 *
 * Author:   Liang Ma (liang-ma@polito.it)
 *
 *----------------------------------------------------------------------------
 */
#include <cmath>
#include <cstdlib>
#include "RNG.h"

template<typename T>
void blackEuro(T* calls, T *puts, 
        T stock, 
        T strike,
        T rate,
        T volatility,
        T time,
        int partitions = 1024,
        int paths = 65536){
    T Dt=time/partitions;

    const T ratio=exp(-rate*time);

    T volDt = volatility * sqrtf(Dt);

#pragma omp parallel for
    for(int sim=0;sim<paths;sim+=2){
        RNG<T> rng(rand());
        T prices1= stock;
        T prices2= stock;
        for(int k=0;k<partitions;k++){
            T num1,num2;
            rng.BOX_MULLER(&num1,&num2);
#ifdef ACC
            prices1*=exp((rate-volatility*volatility/2.0f)*Dt+num1*volDt);
            prices2*=exp((rate-volatility*volatility/2.0f)*Dt+num1*volDt);
#else
            prices1*=1.0f + rate*Dt+num1*volDt;
            prices2*=1.0f + rate*Dt+num2*volDt;
#endif
        }
        calls[sim] += ratio*fmax(prices1- strike,0.0f);
        calls[sim] += ratio*fmax(prices2- strike,0.0f);
        puts[sim] += ratio*fmax(strike - prices1,0.0f);
        puts[sim] += ratio*fmax(strike - prices2,0.0f);
    }
}

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
void blackAsian(T* calls, T *puts, 
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
        T price1= stock, ave1 = stock;
        T price2= stock, ave2 = stock;

        for(int k=0;k<partitions;k++)
        {
            T num1,num2;
            rng.BOX_MULLER(&num1,&num2);
#ifdef ACC
            price1 *= expf(ratio2 + num1*volDt);
            ave1 += price1;
            price2 *= expf(ratio2 + num2*volDt);
            ave2 += price2;
#else
            price1 *= 1.0f + rate * Dt +  num1*volDt;
            ave1 += price1;
            price2 *= 1.0f + rate * Dt +  num2*volDt;
            ave2 += price2;
#endif
        }
        calls[sim] = ratio * fmax(ave1 / (1 + partitions) - strike, 0.0f);
        calls[sim + 1] = ratio * fmax(ave2 / (1 + partitions) - strike, 0.0f);
        puts[sim] = ratio * fmax(strike - ave1 / (1 + partitions) , 0.0f);
        puts[sim + 1] = ratio * fmax(strike - ave2 / (1 + partitions) , 0.0f);
    }
}

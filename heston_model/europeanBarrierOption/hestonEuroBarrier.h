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
void hestonEuroBarrier(T* calls, T *puts, 
        T stock, 
        T strike,
        T rate,
        T volatility,
        T time,
        T rho,
        T kappa,
        T theta,
        T xi,
        T upBar,
        T lowBar,
        int partitions,
        int paths){

	T Dt = time / partitions;
	
	const T ratio1 = exp(-rate*time), ratio2 = sqrtf(1-rho*rho);

#pragma omp parallel for
        for (int sim = 0; sim<paths; sim ++){
            RNG<T> rng(rand());
            T prices = stock;
            T vols = fmax(volatility, 0);

            for (int k = 0; k<partitions; k++){
                T num1, num2;
                T volDt = sqrtf(vols * Dt);

                rng.BOX_MULLER(&num1, &num2);
                prices *= 1.0f + rate * Dt + volDt * (num1*rho + num2*ratio2);
                if (prices > upBar || prices < lowBar){
                    prices = strike;
                    break;
                }
                vols += kappa*(theta - vols)*Dt + xi * num1 * volDt;
                vols = fmax(vols, 0.0f);
            }
            calls[sim] = ratio1*(fmax(prices - strike, 0.0f));
            puts[sim] = ratio1*(fmax(strike - prices, 0.0f));
        }
}

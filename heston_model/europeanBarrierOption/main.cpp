#include <iostream>
#include <ctime>
#include <ctime>
#include <chrono>
#include "hestonEuroBarrier.h"
using namespace std;

int main(int argc, char** argv)
{
    srand(time(NULL));
    int paths = 65536;
    int partitions = 1024;
    const float time = 1.0f,
          rate = 0.0f,
          volatility = 0.04f,
          stock = 100.0f,
          strike = 100.0f,
          lowBar = 90,
          upBar = 110;

    const float	rho = 0.0,     // Correlation of asset and volatility
          kappa = 0.5,   // Mean-reversion rate
          theta = 0.04,  // Long run average volatility
          xi = 1;      // "Vol of vol"

    float *calls = new float[paths];
    float *puts = new float[paths];

    auto start  = chrono::high_resolution_clock::now();
    hestonEuroBarrier<float>(calls, puts, stock, strike, rate, volatility, time, rho,
            kappa, theta, xi, upBar, lowBar, partitions, paths);
    float call = 0, put = 0;

#pragma omp parallel for reduction(+:put)
    for(int sim=0;sim<paths;sim++){
        put += puts[sim];
    }
#pragma omp parallel for reduction(+:call)
    for(int sim=0;sim<paths;sim++){
        call += calls[sim];
    }
    call/=paths;
    put/=paths;

    auto finish  = chrono::high_resolution_clock::now();
    chrono::duration<double> t = finish - start;
    cout << "The execution lasts for "<< t.count() <<" s (CPU time)."<<endl;

    cout << "call price is: " << call << '\t'
        <<"put price is: " << put <<endl;

    delete []puts;
    delete []calls;
    return 0;
}

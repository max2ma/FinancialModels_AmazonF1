#include <iostream>
#include <ctime>
#include <ctime>
#include <chrono>
#include "hestonEuro.h"
using namespace std;

int main(int argc, char** argv)
{
    if(argc < 3) return -1;
    int nargs = 0;
    int paths = atoi(argv[++nargs]);
    int partitions = atoi(argv[++nargs]);
    int num_cpus = atoi(argv[++nargs]);
    srand(time(NULL));
    const float time = 1.0f,
          rate = 0.0319f,
          volatility = 0.010201f,
          stock = 100.0f,
          strike = 100.0f;
    const float	rho = -0.7,     // Correlation of asset and volatility
          kappa = 6.21,   // Mean-reversion rate
          theta = 0.019,  // Long run average volatility
          xi = 0.61;      // "volatility of vol"

    float *calls = new float[paths];
    float *puts = new float[paths];

    auto start  = chrono::high_resolution_clock::now();
    hestonEuro<float>(calls, puts, stock, strike, rate, volatility, time, rho,
            kappa, theta, xi, partitions, paths);
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

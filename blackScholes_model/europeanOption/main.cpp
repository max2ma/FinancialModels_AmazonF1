#include <iostream>
#include <ctime>
#include <ctime>
#include <chrono>
#include "blackEuro.h"
using namespace std;

int main(int argc, char** argv)
{
    int paths = 65536;
    int partitions = 4096;
    float *calls = new float[paths];
    float *puts = new float[paths];
    srand(time(NULL));
    auto start  = chrono::high_resolution_clock::now();

    blackEuro<float>(calls, puts, 100, 110, 0.05, 0.2, 1, partitions, paths);
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

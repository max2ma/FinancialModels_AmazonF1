#include <iostream>
#include <ctime>
#include <ctime>
#include <chrono>
#include "omp.h"
#include "blackEuro.h"
using namespace std;

int main(int argc, char** argv)
{
    if(argc < 3) return -1;
    int nargs = 0;
    int paths = atoi(argv[++nargs]);
    int partitions = atoi(argv[++nargs]);
    int num_cpus = atoi(argv[++nargs]);
    omp_set_dynamic(0);
    omp_set_num_threads(num_cpus);

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

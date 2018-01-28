# Monte Carlo financial models on Amazon F1 instances
## Introduction
This repository includes F1-optimized implementations of four Monte Carlo financial models, namely:
  - the European and the Asian options of the the [Black-Scholes model](https://github.com/KitAway/BlackScholes_MonteCarlo), 
  - the European and the European barrier options the [Heston model](https://github.com/KitAway/HestonModel_MonteCarlo). 

## Performance

### On Amazon F1 FPGA
Target frequency is 250MHz. 
Target device is 'xcvu9p-flgb2104-2-i'

| Models | Options | No.random number generators | No.paths | No.partitions | Execution time[s]| Theoretical estimation[s] | LUT | LUTMem | REG | BRAM | DSP | 
|-|-|-|-|-|-|-|-|-|-|-| -|
| Black-Scholes | European option |64|2^{31}|1|0.2|0.14|25% |2.7%|13% |19% | 43%|
| Black-Scholes | Asian option |64|2^{25}| 256|0.74|0.55|25%|2.2%|13%|19%|43%|
| Heston | European option |32|2^{24}|256|1.52|1.14|15% |2.2%|8.2%|8.2%| 26%|
| Heston | European barrier option |32|2^{23}|256|0.75|0.56|14%|2.3%|7.8%|8.0%|26%|

Taking the first application as an example, there initialized 64 random number generator which run in parallel. For 2^31 path simulations, it takes 0.2s on the FPGA. The resource utilization is no more than 43%. So at least 2x performamnce can be achieved on this device. 

Further optimization can be found in the paper [High Performance and Low Power Monte Carlo Methods to Option Pricing Models via High Level Design and Synthesis](http://ieeexplore.ieee.org/abstract/document/7920245/).

### On Amazon F1 CPU

| Models | Options | No.paths | No.partitions | Execution time[s]|
|-|-|-|-|-|
| Black-Scholes | European option |2^{31}|1|139|
| Black-Scholes | Asian option |2^{20}| 256|16|
| Heston | European option |2^{20}|256|28|
| Heston | European barrier option |2^{20}|256|27|

For the first application, it takes around 140s for 2^31 path simulations on the CPU. So the FPGA can achieve at least 3 order of magnitude faster performance than the CPU. 

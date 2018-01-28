# Monte Carlo financial models on Amazon F1 instances
## Introduction
This repository includes F1-optimized implementations of four Monte Carlo financial models, namely:
  - the European and the Asian options of the the [Black-Scholes model](https://github.com/KitAway/BlackScholes_MonteCarlo), 
  - the European and the European barrier options the [Heston model](https://github.com/KitAway/HestonModel_MonteCarlo). 

(see the hyperlinks above for the mathematical theory behind them).

## Usage

### Black-Scholes kernel models

Examples of usage for the European and Asian options:

> blackEuro -s 100 -k 105 -r 0.1 -v 0.15 -t 10
> blackAsian -s 90 -k 110 -r 0.2 -v 0.1 -t 20

Argument |  Meaning and default value
:-------- | :---
-t       |  time period (1.0)
-r       |  interest rate of riskless asset (0.05)
-v 	 |  volatility of the risky asset (0.2)
-s	 |  initial price of the stock (100)
-k       |  strike price for the option (110)

Note: some of the simulation parameters, namely the number of random number generators, the number of simulations running in parallel for a given RNG and the number of time steps, are set, for the sake of efficiency, as compile-time constants in ***"blackScholes.cpp"***. 

Parameter |  information
:-------- | :---
NUM_STEPS    | number of time steps
NUM_RNGS | number of RNGs running in parallel, proportional to the area cost
NUM_SIMS   | number of simulations running in parallel for a given RNG (512 optimizes BRAM usage)

### Heston kernel models

Examples of usage for European and European with barrier options:

> hestonEuro -s 256
> hestonEuroBarrier -s 512

Argument |  Meaning and default value
:-------- | :---
-s       | number of simulation runs N (512)

Note: some of the simulation parameters, namely the number of random number generators and the number of time steps, are set, for the sake of efficiency, as compile-time constants in ***"heston.cpp"***. 

Parameter |  information
:-------- | :---
NUM_STEPS    | number of time steps
NUM_RNGS | number of RNGs running in parallel, proportional to the area cost

## Performance on Amazon F1 FPGA
Target frequency is 250MHz. 
Target device is 'xcvu9p-flgb2104-2-i'

| Models | Options | No.random number generators | No.paths | No.partitions | Execution time on F1 CPU [s] Execution time on FPGA [s] | LUT | LUTMem | REG | BRAM | DSP | 
|-|-|-|-|-|-|-|-|-|-|-| -|
| Black-Scholes | European option |64|2^{31}|1|139|0.2|25% |2.7%|13% |19% | 43%|
| Black-Scholes | Asian option |64|2^{25}| 256|16|0.74|25%|2.2%|13%|19%|43%|
| Heston | European option |32|2^{24}|256|28|1.52|15% |2.2%|8.2%|8.2%| 26%|
| Heston | European barrier option |32|2^{23}|256|27|0.75|14%|2.3%|7.8%|8.0%|26%|

Further informations about the optimizations used in this implementation can be found in the paper [High Performance and Low Power Monte Carlo Methods to Option Pricing Models via High Level Design and Synthesis](http://ieeexplore.ieee.org/abstract/document/7920245/).



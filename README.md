# Monte Carlo financial models on Amazon F1 instances
## Introduction
This repository includes F1-optimized implementations of four Monte Carlo financial models, namely:
  - the European and the Asian options of the the [Black-Scholes model][Black-Scholes Model], 
  - the European and the European barrier options of the [Heston model][Heston Model]. 

## Theory

### Black-Scholes Model
The [Black-Scholes model][Black-Scholes Model], which was first published by Fischer Black and Myron Scholes in 1973, is a well known basic mathematical model describing the behaviour of investment instruments in financial markets. This model focuses on comparing the Return On Investment for one risky asset, whose price is subject to [geometric Brownian motion][geometric Brownian motion] and one riskless asset with a fixed interest rate.

The geometric Brownian behaviour of the price of the risky asset is described by this stochastic differential equation:
![$$dS=rSdt+\sigma SdW_t$$]
where S is the price of the risky asset (usually called stock price), r is the fixed interest rate of the riskless asset, ![$\sigma$] is the volatility of the stock and ![$W_t$] is a [Wiener process][Wiener process].

According to Ito's Lemma, the analytical solution of this stochastic  differential equation is as follows:
![$$ S_{t+\Delta t}=S_te^{(r-\frac{1}{2}\sigma^2)\Delta t+\sigma\epsilon\sqrt{\Delta t} } $$]
where ![$\epsilon\sim N(0,1)$],  (the standard normal distribution).

Entering more specifically into the financial sector operations, two styles of stock transaction [options][option] are considered in this implementation of the Black-Scholes model, namely the European vanilla option and Asian option (which is one of the [exotic options][exotic options]).
[Call options][Call options] and [put options][put options] are defined reciprocally. Given the basic parameters for an option, namely expiration date and strike price, the call/put payoff price could be estimated as follows.
For the European vanilla option, we have:
![$$P_{Call}=max\{S-K,0\}\\P_{put}=max\{K-S,0\}$$]
where S is the stock price at the expiration date (estimated by the model above) and K is the strike price.
For the Asian option, we have:
![$$P_{Call}=max\{\frac{1}{T}\int_0^TSdt-K,0\}\\P_{put}=max\{K-\frac{1}{T}\int_0^TSdt,0\}$$]
where T is the time period (between now and the option expiration date) , S is the stock price at the expiration date, and K is the strike price.

### Heston Model
The [Heston model][Heston Model], which was first published by Steven Heston in 1993, is a more sophisticated  mathematical model describing the behaviour of investment instruments in financial markets. This model focuses on comparing the Return On Investment for one risky asset, whose price and volatility are subject to [geometric Brownian motion][geometric Brownian motion] and one riskless asset with a fixed interest rate.

Two styles of stock transaction [options][option] are considered in this implementation of the Heston model, namely the European vanilla option and European barrier option (which is one of the [exotic options][exotic options]).
[Call options][Call options] and [put options][put options] are defined reciprocally. Given the basic parameters for an option, namely expiration date and strike price, the call/put payoff price can be estimated as discussed in [Heston model][this article].

## Usage

### Black-Scholes models

Examples of usage for the European and Asian options:
  ```
  > blackeuro 
  > blackasian 
  ```
  
The outputs of both commands are the expected call and put prices. The ```-b <binary_file_name>``` option can be used to specify a binary file name different from the default ```<kernel_name>.hw.xilinx_xil-accel-rd-ku115_4ddr-xpr.awsxclbin```

The model parameters are specified in a file (in protobuf form) called ```blackEuro.parameters``` and ```blackAsian.parameters``` respectively. The meaning of the parameters is as follows.

Parameter |  Meaning and default value
:-------- | :---
time      |  time period (1.0)
rate       |  interest rate of riskless asset (0.05)
volatility|  volatility of the risky asset (0.2)
initprice	 |  initial price of the stock (100)
strikeprice       |  strike price for the option (110)

### Heston models

Examples of usage for European and European with barrier options:
  ```
  > hestoneuro 
  > hestoneurobarrier
  ```

The outputs of both commands are the expected call and put prices. The ```-b <binary_file_name>``` option can be used to specify a binary file name different from the default ```<kernel_name>.hw.xilinx_xil-accel-rd-ku115_4ddr-xpr.awsxclbin```

The model parameters are specified in a file (in protobuf form) called ```hestonEuro.parameters``` and ```hestonEuroBarrier.parameters``` respectively. The meaning of the parameters is as follows (see also [Heston Model][this article] for more details).

Parameter |  Meaning and default value
:-------- | :---
time      |  time period (1.0)
rate       |  interest rate of riskless asset (0.05)
volatility|  volatility of the risky asset (0.3)
initprice	 |  initial price of the stock (100)
strikeprice       |  strike price for the option (110)
theta | long run average price volatility (0.019)
kappa | rate at which the volatility reverts to theta (6.21)
xi | volatility of the volatility (0.61)
rho | covariance (-0.7)
upb | upper bound on price (110)
lowb | lower bound on price (99)

## Performance on Amazon F1 FPGA

Target frequency is 250MHz. 
Target device is 'xcvu9p-flgb2104-2-i'

| Models | Options | No.random number generators | No.paths | No.partitions | Execution time on F1 CPU [s] | Execution time on FPGA [s] | LUT | LUTMem | REG | BRAM | DSP | 
|-|-|-|-|-|-|-|-|-|-|-| -|
| Black-Scholes | European option |64|2^{31}|1|139|0.2|25% |2.7%|13% |19% | 43%|
| Black-Scholes | Asian option |64|2^{25}| 256|16|0.74|25%|2.2%|13%|19%|43%|
| Heston | European option |32|2^{24}|256|28|1.52|15% |2.2%|8.2%|8.2%| 26%|
| Heston | European barrier option |32|2^{23}|256|27|0.75|14%|2.3%|7.8%|8.0%|26%|

## Further information and recompilation

Further informations about the optimizations used in this implementation can be found in the paper [High Performance and Low Power Monte Carlo Methods to Option Pricing Models via High Level Design and Synthesis](http://ieeexplore.ieee.org/abstract/document/7920245/).

In all cases, the enclosed Makefile can be used to compile the models. For example:
  ```
  cd blackScholes_model/europeanOption
  source <path to SDSoc v2017.1>/.settings64-SDx.sh
  export SDACCEL_DIR=<path to aws-fpga>/SDAccel
  export COMMON_REPO=$SDACCEL_DIR/examples/xilinx/
  export PLATFORM=xilinx_aws-vu9p-f1_4ddr-xpr-2pr_4_0
  export AWS_PLATFORM=$SDACCEL_DIR/aws_platform/xilinx_aws-vu9p-f1_4ddr-xpr-2pr_4_0/xilinx_aws-vu9p-f1_4ddr-xpr-2pr_4_0.xpfm
  make TARGETS=hw DEVICES=$AWS_PLATFORM all
  ```
compiles the code and generates the F1-targeted bitstream for the European option of the Black-Scholes model. Environment variable ```COMMON_REPO``` must point to the ```examples/xilinx``` sub-directory of the folder where the AWS development kit github has been checked out.

Note that, for the sake of efficient implementation on the FPGA, the simulation parameters which directly affect the amount of parallelism in the implementation are set as compile-time constants in ```blackScholes.cpp```, ```hestonEuro.h``` and ```hestonEuroBarrier.h``` respectively. They are listed below and can be changed by recompiling the kernels and re-generating the AFI.

Parameter |  information
:-------- | :---
NUM_STEPS    | number of time steps
NUM_RNGS | number of RNGs running in parallel, proportional to the area cost
NUM_SIMS   | number of simulations running in parallel for a given RNG (512 ensures a good BRAM usage on a typical FPGA)
NUM_SIMGROUPS  | number of simulation groups (each with ![$NUM\_RNG \cdot NUM\_SIMS$] simulations) running in pipeline, proportional to the execution time

See also these repositories for more information about the compilation and optimization of the kernels:
  - the European and the Asian options of the the [Black-Scholes model](https://github.com/KitAway/BlackScholes_MonteCarlo), 
  - the European and the European barrier options the [Heston model](https://github.com/KitAway/HestonModel_MonteCarlo). 
  
[option]: https://en.wikipedia.org/wiki/Option_style
[exotic options]: https://en.wikipedia.org/wiki/Exotic_option
[Black-Scholes Model]: https://en.wikipedia.org/wiki/Black%E2%80%93Scholes_model
[Heston Model]: https://en.wikipedia.org/wiki/Heston_model
[geometric Brownian motion]: https://en.wikipedia.org/wiki/Geometric_Brownian_motion
[Wiener process]: https://en.wikipedia.org/wiki/Wiener_process
[Call options]: https://en.wikipedia.org/wiki/Call_option
[put options]: https://en.wikipedia.org/wiki/Put_option
[Mersenne Twister]: https://en.wikipedia.org/wiki/Mersenne_Twister
[Monte Carlo]: https://en.wikipedia.org/wiki/Monte_Carlo_method  
[Box Muller transformation]: https://en.wikipedia.org/wiki/Box%E2%80%93Muller_transform

[$\alpha$]:/figures/alpha.PNG
[$N=NUM\_SIMS \cdot NUM\_RNG \cdot NUM\_SIMGROUPS$]:figures/N.PNG
[$\sigma$]:figures/equ_bs.PNG
[$W_t$]:figures/wt.PNG
[$C=M \cdot N$]:figures/cmn.PNG
[$10^9$]:figures/109.PNG
[$2^{19937}-1$]:figures/19937.PNG
[$U_1$,$U_2 \sim U(0,1)$]:figures/u12.PNG
[$Z_1$,$Z_2\sim N(0,1)$]:figures/z12.PNG
[$t\approx1.25ns$]:figures/t125.PNG
[$NUM\_RNG \cdot NUM\_SIMS$]:figures/nn.PNG
[$t=T_s/C$]:figures/tstep.PNG
[$$Z_1=\sqrt{-2ln(U_1)}cos(2\pi U_2)\\Z_2=\sqrt{-2ln(U_1)}sin(2\pi U_2)$$]:/figures/boxm.PNG
[$t\approx\frac{clock\ period}{NUM\_RNGS}$]:/figures/tpro.PNG
[$$t=T/C\approx\alpha$$]:/figures/tmall.PNG
[$$dS=rSdt+\sigma SdW_t$$]:/figures/equ_bs.PNG
[$$ S_{t+\Delta t}=S_te^{(r-\frac{1}{2}\sigma^2)\Delta t+\sigma\epsilon\sqrt{\Delta t} } $$]:/figures/ito.PNG
[$\epsilon\sim N(0,1)$]:/figures/eps.PNG
[$$P_{Call}=max\{S-K,0\}\\P_{put}=max\{K-S,0\}$$]:/figures/euro.PNG
[$$P_{Call}=max\{\frac{1}{T}\int_0^TSdt-K,0\}\\P_{put}=max\{K-\frac{1}{T}\int_0^TSdt,0\}$$]:/figures/asian.PNG
[$$T=\alpha M \cdot N+\beta N+\gamma M+\theta$$]:/figures/tall.PNG

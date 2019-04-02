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
[Call options][Call options] and [put options][put options] are defined reciprocally. Given the basic parameters for an option, namely expiration date and strike price, the call/put payoff price can be estimated as discussed in [this article][Heston model].

## Performance on PYNQ-Z2 platform

Target frequency is 150MHz. 
Target device is ZYNQ Z0702

| Model | Option | N. random number generators | N. simulations | N. steps   | Time PYNQ FPGA [s] | LUT | LUTMem | REG | BRAM | DSP | 
|-|-|-|-|-|-|-|-|-|-|-|
| Black-Scholes | European option  |2|65536| 1024 |0.118|45% |11%| 35% |10% |70%|
| Black-Scholes | Asian option     |2|65536| 1024 |0.117|47% |11%| 36% |13% |68%|
| Heston | European option         |2|65536| 1024 |0.225|49% |15%| 40% |10% |75%|
| Heston | European barrier option |2|65536| 1024 |0.225|50% |15%| 41% |10% |74%|

## Further information and recompilation

Further informations about the optimizations used in this implementation can be found in the paper [High Performance and Low Power Monte Carlo Methods to Option Pricing Models via High Level Design and Synthesis](http://ieeexplore.ieee.org/abstract/document/7920245/).


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

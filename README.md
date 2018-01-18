# Financial Models on Amazon F1
## Introduction
There are two financial models in this repository, [black-scholes model](https://github.com/KitAway/BlackScholes_MonteCarlo) and [heston model](https://github.com/KitAway/HestonModel_MonteCarlo). The links bring you to the original repository of the implementations of each financial model with several options. The detail introduction of each model can also be found. 

Few modifications are made in order to implement them on the Amazon F1.  

## Usage: 
reference to https://github.com/aws/aws-fpga/tree/master/SDAccel
- checkout the Amazon [AWS-FPGA](https://github.com/aws/aws-fpga) by
    ```
    git clone --recursive git@github.com:aws/aws-fpga.git
    ```
  or
    ```
    git clone --recursive https://github.com/aws/aws-fpga.git
    ```
- set the environment as following
  ```
  source <path to SDSoc v2017.1>/.settings64-SDx.sh
  export SDACCEL_DIR=<path to aws-fpga>/SDAccel
  export COMMON_REPO=$SDACCEL_DIR/examples/xilinx/
  export PLATFORM=xilinx_aws-vu9p-f1_4ddr-xpr-2pr_4_0
  export AWS_PLATFORM=$SDACCEL_DIR/aws_platform/xilinx_aws-vu9p-f1_4ddr-xpr-2pr_4_0/xilinx_aws-vu9p-f1_4ddr-xpr-2pr_4_0.xpfm
  ```
- go to any one project directory build the project by:
  ```
  $ make TARGETS=hw DEVICES=$AWS_PLATFORM all
  ```
 - Create an Amazon FPGA Image (AFI) by create_sdaccel_afi.sh
 - Tracking the status of the registered AFI until available
 - Run the FPGA accelerated application on F1

## Performance

### On Amazon F1 FPGA
Target frequency is 250MHz. 
Target device is 'xcvu9p-flgb2104-2-i'

| Models | Options | No.random generators | No.paths | No.partitions | Execution time[s]| Theoretical estimation[s] | LUT | LUTMem | REG | BRAM | DSP | 
|-|-|-|-|-|-|-|-|-|-|-| -|
| Black-Scholes | European option |64|2^{31}|1|0.2|0.14|25% |2.7%|13% |19% | 43%|
| Black-Scholes | Asian option |64|2^{25}| 256|0.74|0.55|25%|2.2%|13%|19%|43%|
| Heston | European option |32|2^{24}|256|1.52|1.14|15% |2.2%|8.2%|8.2%| 26%|
| Heston | European barrier option |32|2^{23}|256|0.75|0.56|14%|2.3%|7.8%|8.0%|26%|

### On Amazon F1 CPU

| Models | Options | No.paths | No.partitions | Execution time[s]|
|-|-|-|-|-|
| Black-Scholes | European option |2^{31}|1|139|
| Black-Scholes | Asian option |2^{20}| 256|16|
| Heston | European option |2^{20}|256|28|
| Heston | European barrier option |2^{20}|256|27|

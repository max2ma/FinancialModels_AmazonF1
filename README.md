# Financial Models on Amazon F1
## Introduction
There are two financial models in this repository, [black-scholes model](https://github.com/KitAway/BlackScholes_MonteCarlo) and [heston model](https://github.com/KitAway/HestonModel_MonteCarlo). The links bring you to the original repository of the implementations of each financial model with several options. The detail introduction of each model can also be found. 

Few modifications are made in order to implement them on the Amazon F1.  

## Usage: 
reference to https://github.com/aws/aws-fpga/tree/master/SDAccel
- checkout the Amazon [aws-fpga](https://github.com/aws/aws-fpga) and install it properly
- go to any one project directory and modify the value of 'COMMON_REPO'( i.e. the path to 'aws-fpga/SDAccel/examples/xilinx/) in the Makefile
- then build the project by:
  ```
  $ make TARGETS=hw DEVICES=$AWS_PLATFORM all
  ```
 - Create an Amazon FPGA Image (AFI) by create_sdaccel_afi.sh
 - Tracking the status of the registered AFI until available
 - Run the FPGA accelerated application on F1

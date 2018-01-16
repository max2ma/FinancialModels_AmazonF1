# Financial Models on Amazon F1
Usage: 
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

COMMON_REPO=../../../../../
COMMON_DIR:=../../common

#Common Includes
include $(COMMON_REPO)/utility/boards.mk
include $(COMMON_REPO)/libs/xcl2/xcl2.mk
include $(COMMON_REPO)/libs/opencl/opencl.mk

# Host Application
run_bs_SRCS=main.cpp $(oclHelper_SRCS) $(xcl2_SRCS)
run_bs_HDRS=$(xcl2_HDRS)
run_bs_CXXFLAGS=-I$(COMMON_DIR) $(opencl_CXXFLAGS) $(xcl2_CXXFLAGS) $(oclHelper_CXXFLAGS) -std=c++0x
run_bs_LDFLAGS=$(opencl_LDFLAGS)

EXES=run_bs

# Kernel
blackEuro_SRCS=blackEuro.cpp $(COMMON_DIR)/stockData.cpp blackScholes.cpp $(COMMON_DIR)/RNG.cpp

KERNEL_NAME:=blackEuro
XOS=$(KERNEL_NAME)

blackEuro_XOS=$(KERNEL_NAME)

XCLBINS=$(KERNEL_NAME)
CLFLAGS=-I$(COMMON_DIR) -I. --kernel $(KERNEL_NAME) --xp "param:compiler.preserveHlsOutput=1" --xp "param:compiler.generateExtraRunData=true" -s 

# check
check_EXE=run_bs
check_XCLBINS=$(KERNEL_NAME) 

CHECKS=check

include $(COMMON_REPO)/utility/rules.mk

# HOST_ARGS = -s 100 -k 110 -r 0.05 -v 0.2 -t 1 -c 6.04 -p 10.65 -a ${XCLBIN} -n ${KERNEL_NAME}


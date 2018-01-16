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
blackAsian_SRCS=blackAsian.cpp $(COMMON_DIR)/stockData.cpp blackScholes.cpp $(COMMON_DIR)/RNG.cpp

XOS=blackAsian

blackAsian_XOS=blackAsian

XCLBINS=blackAsian
CLFLAGS=-I$(COMMON_DIR) --kernel blackAsian  --xp "param:compiler.preserveHlsOutput=1" --xp "param:compiler.generateExtraRunData=true" -s 
# check
check_EXE=run_bs
check_XCLBINS=blackAsian

CHECKS=check

include $(COMMON_REPO)/utility/rules.mk

# HOST_ARGS= -s 100 -k 105 -r 0.1 -v 0.15 -t 10 -c 24.95 -p 0.283 -n blackAsian

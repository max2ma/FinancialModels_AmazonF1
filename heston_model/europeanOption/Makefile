COMMON_REPO=../../SDAccel_Examples
COMMON_DIR:=../../common/

#Common Includes
include $(COMMON_REPO)/utility/boards.mk
include $(COMMON_REPO)/libs/xcl2/xcl2.mk
include $(COMMON_REPO)/libs/opencl/opencl.mk

# Host Application
run_heston_SRCS=main.cpp $(oclHelper_SRCS) $(xcl2_SRCS)
run_heston_HDRS=$(xcl2_HDRS)
run_heston_CXXFLAGS=-I$(COMMON_DIR) $(opencl_CXXFLAGS) $(xcl2_CXXFLAGS) $(oclHelper_CXXFLAGS) -std=c++0x
run_heston_LDFLAGS=$(opencl_LDFLAGS)

EXES=run_heston

# Kernel
hestonEuro_SRCS=hestonEuro.cpp $(COMMON_DIR)/stockData.cpp $(COMMON_DIR)/volatilityData.cpp $(COMMON_DIR)/RNG.cpp $(COMMON_DIR)/barrierData.cpp


XOS=hestonEuro

hestonEuro_XOS=hestonEuro

XCLBINS=hestonEuro
CLFLAGS=-I$(COMMON_DIR) --kernel hestonEuro  --xp "param:compiler.preserveHlsOutput=1" --xp "param:compiler.generateExtraRunData=true" -s
# check
check_EXE=run_heston
check_XCLBINS=hestonEuro

CHECKS=check

include $(COMMON_REPO)/utility/rules.mk

# -a heston.xclbin -n hestonEuro -p 5.9 -c 8.7

COMMON_REPO:=$(AWS_FPGA_REPO_DIR)/SDAccel/examples/xilinx/
COMMON_DIR:=../../common
PLATFORM=xilinx_aws-vu9p-f1_4ddr-xpr-2pr_4_0

#Common Includes
include $(COMMON_REPO)/utility/boards.mk
include $(COMMON_REPO)/libs/xcl2/xcl2.mk
include $(COMMON_REPO)/libs/opencl/opencl.mk

# These must be different
KERNEL_NAME=blackAsian
EXES=blackasian

# Host Application
$(EXES)_SRCS=../main.cpp $(oclHelper_SRCS) $(xcl2_SRCS)
$(EXES)_CXXFLAGS=-I$(COMMON_DIR) -DPLATFORM=\"$(PLATFORM)\" -DKERNEL=\"$(KERNEL_NAME)\" -DKERNEL_NAME=$(KERNEL_NAME) $(opencl_CXXFLAGS) $(xcl2_CXXFLAGS) $(oclHelper_CXXFLAGS) -std=c++0x -O3 
$(EXES)_LDFLAGS=$(opencl_LDFLAGS) 
$(EXES)_HDRS=$(xcl2_HDRS)
ifeq ($(PROTOBUF),1)
$(EXES)_SRCS+=$(COMMON_DIR)/bs.pb.cc 
$(EXES)_CXXFLAGS+=-DUSE_PROTOBUF 
$(EXES)_LDFLAGS+=-lprotobuf
endif


# Kernel
DEVICES=$(COMMON_REPO)/../../aws_platform/$(PLATFORM)/$(PLATFORM).xpfm
$(KERNEL_NAME)_SRCS=$(KERNEL_NAME).cpp $(COMMON_DIR)/stockData.cpp blackScholes.cpp $(COMMON_DIR)/RNG.cpp
XOS=$(KERNEL_NAME)

$(KERNEL_NAME)_XOS=$(KERNEL_NAME)

XCLBINS=$(KERNEL_NAME)
CLFLAGS=-I$(COMMON_DIR) -I. --kernel $(KERNEL_NAME) 
#CLFLAGS+=--xp "param:compiler.preserveHlsOutput=1" --xp "param:compiler.generateExtraRunData=true" -s 

# check
check_EXE=$(KERNEL_NAME)
check_XCLBINS=$(KERNEL_NAME) 

CHECKS=check

# For execution purely in C
$(EXES)_c: $($(KERNEL_NAME)_SRCS) ../main.cpp
	c++ -o $(EXES)_c -DPURE_C $($(EXES)_CXXFLAGS) $($(KERNEL_NAME)_SRCS) ../main.cpp

pure_c: $(EXES)_c
	$(EXES)_c

include $(COMMON_REPO)/utility/rules.mk

# HOST_ARGS = -b xclbin/blackAsian.sw_emu.xilinx_aws-vu9p-f1_4ddr-xpr-2pr_4_0.xclbin -c 3.42 -p 3.25
#


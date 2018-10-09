COMMON_DIR:=../../common
PROFILE:=yes
PLATFORM=$(AWS_PLATFORM)
TARGETS=hw_emu
DEVICES=$(AWS_PLATFORM)
#Common Includes
include $(COMMON_REPO)/utility/boards.mk
include $(COMMON_REPO)/libs/xcl2/xcl2.mk
include $(COMMON_REPO)/libs/opencl/opencl.mk

# These must be different
KERNEL_NAME=blackEuro
EXES=blackeuro

# Host Application
$(EXES)_SRCS=../main.cpp $(oclHelper_SRCS) $(xcl2_SRCS)
$(EXES)_CXXFLAGS=-I$(COMMON_DIR) -DPLATFORM=\"$(PLATFORM)\" -DKERNEL=\"$(KERNEL_NAME)\" -DKERNEL_NAME=$(KERNEL_NAME) $(opencl_CXXFLAGS) $(xcl2_CXXFLAGS) $(oclHelper_CXXFLAGS) 
$(EXES)_LDFLAGS=$(opencl_LDFLAGS) 
$(EXES)_HDRS=$(xcl2_HDRS)


# Kernel
#DEVICES=$(COMMON_REPO)/../../aws_platform/$(PLATFORM)/$(PLATFORM).xpfm
$(KERNEL_NAME)_SRCS=$(KERNEL_NAME).cpp $(COMMON_DIR)/RNG.cpp
XOS=$(KERNEL_NAME)

$(KERNEL_NAME)_XOS=$(KERNEL_NAME)

XCLBINS=$(KERNEL_NAME)
CLFLAGS+=-I$(COMMON_DIR) -I. --kernel $(KERNEL_NAME) 
#CLFLAGS+=--xp "param:compiler.preserveHlsOutput=1" --xp "param:compiler.generateExtraRunData=true" -s 
LDCLFLAGS+=--nk $(KERNEL_NAME):32 --jobs 4

# check
check_EXE=$(KERNEL_NAME)
check_XCLBINS=$(KERNEL_NAME) 

CHECKS=check

include $(COMMON_REPO)/utility/rules.mk

# HOST_ARGS = -b xclbin/blackEuro.sw_emu.xilinx_aws-vu9p-f1_4ddr-xpr-2pr_4_0.xclbin -c 6.04 -p 10.65


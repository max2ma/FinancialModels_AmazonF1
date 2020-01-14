GXX=g++
COMMON_DIR:=../
HDRS=-I$(COMMON_DIR)

KERNEL_NAME:=blackEuro
SRCS=main.cpp $(KERNEL_NAME).h
CFLAGS=-I ../../common/ -O2 -fopenmp -std=c++11 

EXE=run_$(KERNEL_NAME)

PATHS=65536
PARTS=1024
CPUS=4

run: ${EXE}
	@./$(EXE) ${PATHS} ${PARTS} ${CPUS}

$(EXE) :$(SRCS)
	@$(GXX) $(HDRS) $(CFLAGS) -o $@ $^ 

clean:
	@if [ -f $(EXE) ]; then rm $(EXE); fi 

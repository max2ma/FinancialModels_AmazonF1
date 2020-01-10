GXX=g++
COMMON_DIR:=../
HDRS=-I$(COMMON_DIR)

KERNEL_NAME:=hestonEuro
SRCS=main.cpp $(KERNEL_NAME).h
CFLAGS=-I ../../common/ -O2 -fopenmp -std=c++11 

EXE=run_$(KERNEL_NAME)

run: ${EXE}
	./$(EXE)

$(EXE) :$(SRCS)
	$(GXX) $(HDRS) $(CFLAGS) -o $@ $^ 

clean:
	@if [ -f $(EXE) ]; then rm $(EXE); fi 

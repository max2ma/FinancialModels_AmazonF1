GXX=g++
COMMON_DIR:=../
HDRS=-I$(COMMON_DIR)

KERNEL_NAME:=blackEuro
SRCS=main.cpp $(KERNEL_NAME).h

EXE=run_$(KERNEL_NAME)

$(EXE) :$(SRCS)
	$(GXX) $(HDRS) -O2 -o $@ $^ -std=c++11 -fopenmp -I ../../common/
	./$(EXE)

clean:
	@if [ -f $(EXE) ]; then rm $(EXE); fi 

CC      = g++
LDFLAGS = -lboost_program_options

all: ecx-estimator clean

ecx-estimator: ecx-estimator.o
	$(CC) -o $@ $^ $(LDFLAGS)

ecx-estimator.o: ecx-estimator.cpp 
	$(CC) -c $< 

.PHONY: clean cleanest

clean:
	rm *.o

cleanest:
	rm ecx-estimator.exe
CC = g++
FLAGS = -std=c++11 -g
OBJ = vm_sim.o tlb.o

vm_sim : ${OBJ}
	${CC} ${FLAGS} ${OBJ} -o vm_sim

vm_sim.o : vm_sim.cpp
	${CC} ${FLAGS} -c vm_sim.cpp

tlb.o : tlb.cpp tlb.hpp
	${CC} ${FLAGS} -c tlb.cpp

clean:
	rm *.o

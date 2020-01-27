all: main

main: main.o chunk.o predicate.o
	g++ -o main main.o chunk.o predicate.o

chunk.o: chunk.cpp chunk.hpp predicate.hpp
	g++ -c chunk.cpp -O2 -std=gnu++17

main.o: main.cpp chunk.hpp predicate.hpp
	g++ -c main.cpp -O2 -std=gnu++17

predicate.o: predicate.cpp predicate.hpp
	g++ -c predicate.cpp -O2 -std=gnu++17

clean:
	rm -rf *.o main

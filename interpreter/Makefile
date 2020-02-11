all: main

main: main.o bytestream.o node.o
	g++ -o main main.o bytestream.o node.o

main.o: main.cpp
	g++ -c main.cpp -O2 -std=gnu++17

bytestream.o: bytestream.cpp bytestream.hpp predicate.hpp
	g++ -c bytestream.cpp -O2 -std=gnu++17

node.o: node.cpp node.hpp predicate.hpp
	g++ -c node.cpp -O2 -std=gnu++17

clean:
	rm -f *.o main

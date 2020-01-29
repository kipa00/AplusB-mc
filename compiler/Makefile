all: main

main: main.o chunk.o predicate.o bitstream.o
	g++ -o main main.o chunk.o predicate.o bitstream.o

chunk.o: chunk.cpp chunk.hpp bitstream.hpp predicate.hpp
	g++ -c chunk.cpp -O2 -std=gnu++17

main.o: main.cpp chunk.hpp predicate.hpp
	g++ -c main.cpp -O2 -std=gnu++17

predicate.o: predicate.cpp predicate.hpp
	g++ -c predicate.cpp -O2 -std=gnu++17

bitstream.o: bitstream.cpp bitstream.hpp predicate.hpp
	g++ -c bitstream.cpp -O2 -std=gnu++17

clean:
	rm -rf *.o main

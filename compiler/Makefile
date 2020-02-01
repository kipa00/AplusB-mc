all: main

main: main.o chunk.o predicate.o bitstream.o world.o analyze.o block.o
	g++ -o main main.o chunk.o predicate.o bitstream.o world.o analyze.o block.o -lz

chunk.o: chunk.cpp chunk.hpp bitstream.hpp predicate.hpp
	g++ -c chunk.cpp -O2 -std=gnu++17

main.o: main.cpp world.hpp predicate.hpp
	g++ -c main.cpp -O2 -std=gnu++17

predicate.o: predicate.cpp predicate.hpp
	g++ -c predicate.cpp -O2 -std=gnu++17

bitstream.o: bitstream.cpp bitstream.hpp predicate.hpp
	g++ -c bitstream.cpp -O2 -std=gnu++17

world.o: world.cpp world.hpp chunk.hpp predicate.hpp
	g++ -c world.cpp -O2 -std=gnu++17

analyze.o: analyze.cpp analyze.hpp world.hpp predicate.hpp
	g++ -c analyze.cpp -O2 -std=gnu++17

block.o: block.cpp block.hpp predicate.hpp
	g++ -c block.cpp -O2 -std=gnu++17

clean:
	rm -f *.o main

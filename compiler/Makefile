all: main

main: main.o chunk.o predicate.o bitstream.o world.o analyze.o check.o block.o color.o
	g++ -o main main.o chunk.o predicate.o bitstream.o world.o analyze.o check.o block.o color.o -lz

chunk.o: chunk.cpp chunk.hpp bitstream.hpp predicate.hpp
	g++ -c chunk.cpp -O2 -std=gnu++17

main.o: main.cpp world.hpp predicate.hpp check.hpp color.hpp
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

check.o: check.cpp check.hpp world.hpp predicate.hpp block.hpp
	g++ -c check.cpp -O2 -std=gnu++17

color.o: color.cpp color.hpp predicate.hpp
	g++ -c color.cpp -O2 -std=gnu++17

clean:
	rm -f *.o main

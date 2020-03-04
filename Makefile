all: target/mc-run target/mc-compile

target/mc-run:
	cd interpreter && make
	mv interpreter/mc-run target/

target/mc-compile:
	cd compiler && make
	mv compiler/mc-compile target/

clean:
	cd interpreter && make clean
	cd compiler && make clean
	rm -f target/*

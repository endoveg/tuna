all:
	g++ main.cpp fft.cpp ../audio/sampling.cpp -lpthread -lasound -std=c++11 -D BOOSTED -g -o boosted
	g++ main.cpp ../audio/sampling.cpp -lpthread -lasound -std=c++11 -g -o single
yin_test: yin_test.o sampling.o yin.o
	g++ -o yin_test yin_test.o ../audio/sampling.o yin.o -lpthread -lasound
sampling.o:
	g++ -c ../audio/sampling.cpp
yin.o:
	g++ -Wall -c yin.cpp
yin_test.o:
	g++ -c yin_test.cpp
plot:
	./yin_test > output.dat
	octave-cli plotter.m

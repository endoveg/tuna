all:
	g++ main.cpp fft.cpp ../audio/sampling.cpp -lpthread -lasound -std=c++11 -D BOOSTED -g -o boosted
	g++ main.cpp ../audio/sampling.cpp -lpthread -lasound -std=c++11 -g -o single
yin_test: yin_test.o sampling.o yin.o
	g++ -o yin_test yin_test.o ../audio/sampling.o yin.o -lpthread -lasound
yin_test_rec: yin_test.o sampling.o yin_rec.o
	g++ -o yin_test_rec yin_test.o ../audio/sampling.o yin_rec.o -lpthread -lasound
sampling.o:
	g++ -c ../audio/sampling.cpp -o ../sampling.h
yin.o:
	g++ -Wall -c yin.cpp
yin_rec.o:
	g++ -Wall -DREC -c yin.cpp -o yin_rec.o
yin_test.o:
	g++ -c yin_test.cpp
plot:
	./yin_test > output.dat
	octave-cli plotter.m

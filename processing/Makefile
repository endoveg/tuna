all:
	g++ main.cpp fft.cpp ../audio/sampling.cpp -lpthread -lasound -std=c++11 -D BOOSTED -g -o boosted
	g++ main.cpp ../audio/sampling.cpp -lpthread -lasound -std=c++11 -g -o single
yin_test: yin_test.o sampling.o yin.o
	g++ -o yin_test yin_test.o ../audio/sampling.o yin.o -lpthread -lasound
yin_test_rec: yin_test.o sampling.o yin_rec.o
	g++ -o yin_test_rec yin_test.o ../audio/sampling.o yin_rec.o -lpthread -lasound
sampling.o:
	g++ -c -DAUDIO_DEVICE="plughw:0,1" ../audio/sampling.cpp -o ../audio/sampling.o
yin.o:
	g++ -ggdb -Wall  -c yin.cpp
yin_rec.o:
	g++ -Wall -DRECURSIVE -c yin.cpp -o yin_rec.o
yin_test.o:
	g++ -c yin_test.cpp
plot:
	./yin_test > output.dat
	octave-cli plotter.m
filter.o:
	g++ -ggdb -c filter.cpp
filter_test.o: 
	g++ -ggdb -c filter_test.cpp
filter_test: filter.o yin.o filter_test.o sampling.o
	g++ -ggdb filter.o yin.o filter_test.o ../audio/sampling.o -o filter_test -lpthread -lasound
filter_coef:
	octave-cli make_filter.m > coef
yin_with_ring: sampling.o filter.o yin.o
	g++ -ggdb ring_buff.h filter.o yin.o yin_with_ring.cpp ../audio/sampling.cpp -lpthread -std=c++11 -lasound -g -o yin_with_ring

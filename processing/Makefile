all:
	g++ main.cpp fft.cpp ../audio/sampling.cpp -lpthread -lasound -std=c++11 -D BOOSTED -g -o boosted
	g++ main.cpp ../audio/sampling.cpp -lpthread -lasound -std=c++11 -g -o single
sampling.o:
	g++ -c  ../audio/sampling.cpp -o ../audio/sampling.o
yin.o:
	g++ -ggdb -Wall  -c yin.cpp
yin_test.o:
	g++ -c yin_test.cpp
filter.o:
	g++ -ggdb -c filter.cpp
filter_test.o: 
	g++ -ggdb -c filter_test.cpp
filter_test: filter.o yin.o filter_test.o sampling.o filter_coef
	g++ -ggdb filter.o yin.o filter_test.o ../audio/sampling.o -o filter_test -lpthread -lasound
filter_coef:
	python3 filter.py 
yin_with_ring: sampling.o filter.o yin.o filter_coef
	g++ ring_buff.h filter.o yin.o yin_with_ring.cpp ../audio/sampling.o -lpthread -lasound -o yin_with_ring

all:
	g++ main.cpp fft.cpp ../audio/sampling.cpp -lpthread -lasound -std=c++11 -D BOOSTED -g -o boosted
	g++ main.cpp ../audio/sampling.cpp -lpthread -lasound -std=c++11 -g -o single
	g++ ring_buff.h fft.cpp test_ring.cpp ../audio/sampling.cpp -lpthread -std=c++11 -lasound -g -o ring

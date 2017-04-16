all:
	g++ main.cpp fft.cpp ../audio/sampling.cpp -lasound -lpthread -std=c++11 -o parallel
	g++ main.cpp ../audio/sampling.cpp -lasound -lpthread -std=c++11 -D SINGLE_CORE -o single

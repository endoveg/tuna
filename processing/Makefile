all:
	g++ main.cpp fft.cpp ../audio/sampling.cpp -lpthread -lasound -std=c++11 -D BOOSTED -g -o boosted
	g++ main.cpp ../audio/sampling.cpp -lpthread -lasound -std=c++11 -g -o single

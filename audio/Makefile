test_sampling: test_sampling.o sampling.o
	g++ -ggdb -o test_sampling sampling.o test_sampling.o -lpthread -lasound
sampling.o:
	g++ -ggdb -c sampling.cpp
test_sampling.o:
	g++ -ggdb -c test_sampling.cpp -lpthread

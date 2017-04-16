#ifndef FFT_H
#define FFT_H 

#include <vector>
#include <complex>
#include "../audio/sampling.h"
/*
This class is to be passed to calculate() function in pthread_create (see fft.h)
it is quite akward, but it is becasuse of strange design of pthread_create 
*/
class sum_info
{
public:
	unsigned int sample_nu; //the number of data points analysed
	unsigned int core_nu;
	amplitude_probes raw; //data to be analysed
	std::vector <std::complex <double> > *odds; // an array for odd summs
	std::vector <std::complex <double> > *evens;// an array for even sums
	std::vector <std::complex <double> > *image;// the result of FFT
	unsigned int my_nu; //the number of the thread
	sum_info ();
	sum_info (amplitude_probes raw, std:: vector <std::complex <double> > *image, std::vector <std::complex <double> > *odds, std::vector <std::complex <double> > *evens, unsigned int sample_nu, unsigned int core_nu, unsigned int my_nu);
};

void dft (amplitude_probes& proto, std::vector <std::complex <double> > &image, const unsigned int core_nu);
void* calculate (void* info);

#endif

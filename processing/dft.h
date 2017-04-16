#ifndef FFT_H
#define FFT_H 

#include <vector>
#include <complex>
#include "../audio/sampling.h"

void fft (amplitude_probes& proto, std::vector <std::complex <double> > &image);

#endif

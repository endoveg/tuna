#ifndef FFT_H
#define FFT_H 

#include <vector>
#include <complex>
#include "../audio/sampling.h"

void fft(amplitude_probes& proto, std::vector <std::complex <double> > &image, unsigned int N);
void real_fft(amplitude_probes& proto, std::vector <std::complex <double> > &image, unsigned int sample_nu, unsigned int indent);
#endif

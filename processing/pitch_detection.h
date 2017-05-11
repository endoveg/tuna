#ifndef PITCH_DETECTION
#define PITCH_DETECTION
#include <complex>
#include <vector>
#include "../audio/sampling.h"

void processor (amplitude_probes& input, std::vector <std::complex <double> >& image,
                std::vector <std::complex <double> >& expos, std::vector <double>& ampl, 
                unsigned int wsize, unsigned int f_d, std::vector <double>& freq, unsigned int step);
unsigned int PitchDetection(std::vector <double>& spectrum_amplitude, unsigned int N, double f_d, std::vector <double>& frequency);

#endif

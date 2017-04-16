#include <complex>
#include <iostream>
#include <cmath>
#include <vector>
#include <fstream>
#include <errno.h>
#include <unistd.h>
#include "dft.h"
#include "../audio/sampling.h"

#ifdef SINGLE_CORE
void fft (amplitude_probes& proto, std::vector <std::complex <double> > &image)
{
	for (unsigned int i = 0; i < proto.count; i++)
		for (unsigned int j = 0; j < proto.count - 1; j++)
		{
			image [i] += std::complex <double>(proto [j]) * exp (std::complex <double> (0, -2 * M_PI * j * i / proto.count));  
		}
}
#endif

int main ()
{
	#ifndef SINGLE_CORE
	const unsigned int cores = sysconf (_SC_NPROCESSORS_ONLN);
	#endif
	const unsigned long int sz = 4096; 
	const unsigned long int discr_freq = 44100;
	std::vector <double> freq (sz/2);
	for (unsigned int i = 0; i < sz / 2; i++)
		freq[i] = (double) discr_freq / sz * i;
	amplitude_probes amp = amplitude_probes (44100, sz, 2);
	syncmaster* sync = amp.capture (1);
	while (pthread_mutex_trylock (&sync->is_done) == EBUSY)
	{
	}
	std::vector <std::complex <double> > out (sz);
	#ifdef SINGLE_CORE
	fft (amp, out);
	#else 
	dft (amp, out, cores);
	#endif
	{
		std::ofstream display;
		display.open ("output.txt");
		for (unsigned int i = 0; i < sz/2; i++)
			display << freq[i] <<" "<<std::abs (out[i]) <<"\n";
		display.close ();
	}
	return 0;
}


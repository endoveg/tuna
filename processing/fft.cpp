#include <complex>
#include <cmath>
#include <vector>
#include <iostream>
#include <pthread.h>
#include "dft.h"
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include "../audio/sampling.h"

sum_info::sum_info (): raw (0, 0, 0), image (NULL), odds (NULL), evens (NULL), sample_nu (0), core_nu (0), my_nu (0)
{
}

sum_info::sum_info (amplitude_probes r, std::vector <std::complex <double> > *i, std::vector <std::complex <double> > *o,
		    std::vector <std::complex <double> > *e, unsigned int s, unsigned int c, unsigned int m): raw (r), image (i), odds (o), evens (e), sample_nu (s), core_nu (c), my_nu (m)
{
}
//using Cooley-Tukey algorithm
void* calculate (void* info)
{
	sum_info* dat = (sum_info*) info;
	for (unsigned int i = dat->my_nu; i < dat->sample_nu; i += dat->core_nu)
	{
		for (unsigned int j = 0; j < dat->sample_nu/2; j++)
		{
			(*(dat->evens))[i] += std::complex <double> (dat->raw[2 * j]) * exp (std::complex <double> (0, -4 * M_PI * j * i /dat->sample_nu));
			(*(dat->odds))[i] += std::complex <double> (dat->raw[2 * j + 1]) * exp (std::complex <double> (0, -4 * M_PI * j * i / dat->sample_nu));
		}
	}
	for (unsigned int i = dat->my_nu; i < dat->sample_nu / 2; i += dat->core_nu)
	{
		(*(dat->image)) [i] = (*(dat->evens))[i] + exp(std::complex <double>(0, -2 * M_PI * i / dat->sample_nu)) * (*(dat->odds)) [i];
		(*(dat->image)) [i + dat->sample_nu / 2] = (*(dat->evens))[i] - exp(std::complex <double>(0, -2 * M_PI * i / dat->sample_nu)) * (*(dat->odds)) [i];

	}
	return NULL;
}	

void dft(amplitude_probes& proto, std::vector <std::complex <double> > &image, const unsigned int core_nu)
{
	const unsigned int sample_nu = proto.count;
	std::vector <std::complex <double> > odds (sample_nu);
	std::vector <std::complex <double> > evens (sample_nu);
	std::vector <pthread_t> thr (core_nu);
	std::vector <sum_info> inf (core_nu);
	for (unsigned int i = 0; i < core_nu; i++)
	{
		inf[i] = sum_info (proto, &image, &odds, &evens, sample_nu, core_nu, i);
		errno = pthread_create (thr.data() + i, NULL, calculate, (void*) (&inf[i]));
		if (errno)
		{
			std::cout << strerror (errno);
			return;
		}
	}
	for (unsigned int i = 0; i < core_nu; i++)
	{
		errno = pthread_join (thr [i], NULL);
		if (errno)
		{
			std::cout << strerror (errno);
			return;
		}
	}
}

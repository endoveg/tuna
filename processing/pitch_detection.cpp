#include <complex>
#include <iostream>
#include <cmath>
#include <vector>
#include <fstream>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include "../audio/sampling.h"
#include "dft.h"

double f(int i, double f_0)
{
	return f_0 * pow(2, (double)i / 12);
}

double Extinguishing(double* tempered_spectrum_amplitude, int i, const double extinguish_level)
{
	double extinguished_amplitude = tempered_spectrum_amplitude[i];
	for(int j = 2; j <= 8; j++)
		if(i - (int)(12 * log2((double)j) + 0.5) >= 0)
			extinguished_amplitude -= extinguish_level * tempered_spectrum_amplitude[i - (int)(12 * log2((double)j) + 0.5)];
	if(extinguished_amplitude > 0)
		return extinguished_amplitude;
	else
		return 0;
}

//N is halfed wsize!
unsigned int PitchDetection(std::vector <double>& spectrum_amplitude, unsigned int N, double f_d, std::vector <double>& frequency)
{
	const double f_0 = 440.0;
	const double delta_f = f_d / 2 / N;

	const int min_index = -32;
	const int max_index = (int)(12 * log2 (f_d / f_0 ) - 13);

	const double subtrahend = spectrum_amplitude[(unsigned int)(f(min_index, f_0) / delta_f)];
	for(unsigned int i = 0; i < N; i++)
	{
		if(spectrum_amplitude[i] < subtrahend)
			spectrum_amplitude[i] = 0;
		else
			spectrum_amplitude[i] -= subtrahend;
	}

	double* tempered_spectrum_amplitude = (double*)calloc(max_index - min_index + 1, sizeof(double));

	unsigned int j_min, j_max, j;
	for(int i = min_index; i <= max_index; i++)
	{
		j_min = (unsigned int)(f(i, f_0) + f(i - 1, f_0)) / 2 / delta_f + 1;
		j_max = (unsigned int)(f(i, f_0) + f(i + 1, f_0)) / 2 / delta_f;
		for(j = j_min; j <= j_max; j++)
			tempered_spectrum_amplitude[i - min_index] += spectrum_amplitude[j];
	}

	double* extinguished_spectrum_amplitude = (double*)malloc((max_index - min_index + 1) * sizeof(double));
	for(int i = 0; i <= max_index - min_index; i++)
		extinguished_spectrum_amplitude[i] = Extinguishing(tempered_spectrum_amplitude, i, 3.0);

	double target_i = min_index;
	double max_amplitude = 0;
	for(int i = min_index; i <= max_index; i++)
	{
		if(extinguished_spectrum_amplitude[i - min_index] > max_amplitude)
		{
			max_amplitude = extinguished_spectrum_amplitude[i - min_index];
			target_i = i;
		}
	}
	j_min = (unsigned int)(f(target_i, f_0) + f(target_i - 1, f_0)) / 2 / delta_f + 1;
	j_max = (unsigned int)(f(target_i, f_0) + f(target_i + 1, f_0)) / 2 / delta_f;
    unsigned int target_j = j_min;
	max_amplitude = 0;
	for(j = j_min; j <= j_max; j++)
	{
		if(spectrum_amplitude[j] > max_amplitude)
		{
			max_amplitude = spectrum_amplitude[j];
			target_j = j;
		}
	}

	free(tempered_spectrum_amplitude);
	free(extinguished_spectrum_amplitude);
    if (target_j > N)
    {
        std::cout<<"PitchDerection Error"<<std::endl;
        exit(-1);
    }
	return target_j;
}

double princarg (double phase)
{
    return fmod(phase + M_PI, -2 * M_PI) + M_PI;
}
/*
For the pitch correction see http://www.music.mcgill.ca/~ich/classes/dafx_book.pdf
pp. 337-339
*/

//this function is not used yet in the code due to unstable results
//however, it is left here for further anlysis

double pitch_correction (unsigned int candidate, std::vector <std::complex <double> > first, std::vector <std::complex <double> > second,
                         unsigned int delay, unsigned int N, double f_d)
{
    double phi1 = std::arg (first[candidate]);
    double phi2 = std::arg (second[candidate]);
    double phi2t = phi1 + 2 * M_PI / N * candidate * delay; 
    double phi2err = princarg (phi2 - phi2t);
    double phi2u = phi2t + phi2err;
    return (phi2u - phi1) * f_d / delay / 2 / M_PI; 
}

/*
For sliding DFT see http://www.comm.toronto.edu/~dimitris/ece431/slidingdft.pdf
For moving Hamming or Hanning windows see http://ptgmedia.pearsoncmg.com/images/9780137027415/samplepages/0137027419.pdf
pp. 409-410
or http://www.ingelec.uns.edu.ar/pds2803/materiales/articulos/slidingdft_bw.pdf
pp. 78
*/
void shifter (amplitude_probes& input, unsigned int k, std::vector <std::complex <double> >& image, std::vector<double> ampl, std::vector <std::complex <double> >& expos, unsigned int wsize, unsigned int shift)
{
    if (image.size() > wsize / 2)
    {
        std::cout<<"Incorrect input vector or window size"<<std::endl;
        exit(-1);
    }
    if (shift >= wsize / 2)
    {
        std::cout<<"Too large shift"<<std::endl;
        exit(-1);
    } 
    if (expos.size() < shift * wsize / 2)
    {
        std::cout<<"Not enough expos"<<std::endl;
        exit(-1);
    }
    std::complex <double> tmp (0, 0);
    for (unsigned int j = 0; j < wsize / 2; j++)
    {
        tmp = image[j] + std::complex <double> (input[k + wsize] - input[k]);
        for (unsigned int i = 1; i < shift; i++)
            tmp += std::complex <double> (input[k + i + wsize] - input[k + i]) * expos [i * j + i - 1];
        image[j] *= expos [shift * j + shift - 1];
    }
    //applying Hamming window
    for (unsigned int i = 1; i < wsize / 2 - 1; i++)
        ampl[i] = std::abs(0.54 * image[i] - 0.23 * (image[i - 1] + image[i + 1])); 

}

void processor (amplitude_probes& input, std::vector <std::complex <double> >& image,
                std::vector <std::complex <double> >& expos, std::vector <double>& ampl, 
                unsigned int wsize, unsigned int f_d, std::vector <double>& freq, unsigned int step)
{
    if (input.count - wsize <= 0)
    {
        std::cout<<"Incorrect window size"<<std::endl;
        exit (-1);
    }
    real_fft (input, image, wsize, 0);
    for (unsigned int i = 0; i < wsize / 2; i ++)
        ampl[i] = std::abs (image[i]);
    unsigned int cand = PitchDetection (ampl, wsize / 2, f_d, freq);
    double delta;
    std::cout<<freq[cand]<<std::endl;
    for (unsigned int k = 0; k < input.count - wsize; k++)
    {
        shifter (input, k, image, ampl, expos, wsize, step);
        cand = PitchDetection (ampl, wsize / 2, f_d, freq);
        //for interpolation see http://cds.cern.ch/record/738182/files/ab-2004-023
        if (2 * ampl[cand] > ampl[cand - 1] + ampl[cand + 1])
        {
            delta = log (ampl[cand + 1] / ampl[cand - 1]) / 2 / log (ampl[cand] * ampl[cand] / ampl[cand + 1] / ampl[cand - 1]);
            if (std::isnan(delta))
                delta = 0;
        }
        else 
            delta = 0;
        std::cout<<(double)f_d / (double)wsize * (double)cand + delta<<std::endl;
    }
}

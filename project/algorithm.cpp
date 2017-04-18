#include <complex>
#include <iostream>
#include <cmath>
#include <vector>
#include <fstream>
#include <stdint.h>
#include <stdlib.h>


double f(int i, double f_0)
{
	return f_0 * pow(2, (double)i / 12);
}

double Extinguishing(double* tempered_spectrum_amplitude, int i, const double extinguish_level)
{
	double extinguished_amplitude = tempered_spectrum_amplitude[i];
	for(int j = 1; j <= 3; j++)
		if(i - 12 * j >= 0)
			extinguished_amplitude -= extinguish_level * tempered_spectrum_amplitude[i - 12 * j];
	if(extinguished_amplitude > 0)
		return extinguished_amplitude;
	else
		return 0;
}

double PitchDetection(double* frequency, double* spectrum_amplitude, const unsigned int N, const double f_d)
{
	const double f_0 = 440.0;
	const double delta_f = f_d / 2 / N;

	const int min_index = -45;
	const int max_index = 67;

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
		extinguished_spectrum_amplitude[i] = Extinguishing(tempered_spectrum_amplitude, i, 1.0);

	///////////////////////
	std::ofstream output;
	output.open("output.txt");
	for(int i = min_index; i <= max_index; i++)
		output << f(i, f_0) << " " << extinguished_spectrum_amplitude[i - min_index] << "\n";
	output.close();
	///////////////////////

	double target_i;
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
	unsigned int target_j;
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

	return frequency[target_j];
}

int main()
{
	const unsigned int N = 1024;
	const double f_d = 44100.0;

	double* frequency = (double*)malloc(N * sizeof(double));
	double* spectrum_amplitude = (double*)malloc(N * sizeof(double));

	std::ifstream source;
	source.open("e.txt");
	for(unsigned int i = 0; i < N; i++)
		source >> frequency[i] >> spectrum_amplitude[i];
	source.close();

	std::cout << "frequency: " << PitchDetection(frequency, spectrum_amplitude, N, f_d) << " Hz\n";

	free(frequency);
	free(spectrum_amplitude);
	return 0;
}


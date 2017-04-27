#include "../audio/sampling.h"
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <vector>

/* modified autocorrelation function
I WON'T COPY IT
 */
#ifndef YIN_H
#define YIN_H


class acf {
 public:
  acf(amplitude_probes& amp, unsigned int W) {
    window_size = W;
    A = &amp;
  }
  double at(unsigned int time, unsigned int lag);
  ~acf() {
  }
  unsigned int get_size_of_window() {
    return window_size;
  }
 private:
  amplitude_probes * A;
  unsigned int window_size;
  unsigned long int *value;
};
std::vector <double> diff(amplitude_probes& A, unsigned int time, acf& ACF);
std::vector <double> norm (std::vector <double>& d_of_time, unsigned  int W);
unsigned long int abs_threshold(std::vector <double>& normalized, unsigned int W, float threshold);
double yin(amplitude_probes& amp, float threshold, unsigned int window_size,
	   unsigned int time);
void yin_tuner(amplitude_probes& amp, float threshold, unsigned int window_size);
double quadratic_interpolation(amplitude_probes& amp, unsigned int i, unsigned int w);

#endif

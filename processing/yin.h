#include "../audio/sampling.h"
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <vector>

/* modified autocorrelation function
I WON'T COPY IT
 */


class acf {
 public:
  acf(amplitude_probes *amp, unsigned int W) {
    //unsigned long int size = window_size * window_size;
    //value = (long int*)malloc(sizeof(long int) * size);
    //if (value == NULL) {
    //  printf("why?\n");
    // }
    //is_calculated = (short int *)calloc(size, sizeof(short int));
    window_size = W;
    A = amp;
  }
  short int b_index(unsigned long int x, unsigned long int y) {
    return *(is_calculated + x + y * window_size);
  }
  long int index(unsigned long int x, unsigned long int y) {
    return *(is_calculated + x + y * window_size);
  }
  long long at(unsigned long int time, unsigned long int lag);
  ~acf() {
    //free(is_calculated);
    //free(value);
  }
  unsigned long int get_size_of_window() {
    return window_size;
  }
 private:
  amplitude_probes * A;
  unsigned int window_size;
  short int  *is_calculated;
  unsigned long int *value;
};
std::vector <unsigned long long> diff(acf& ACF, unsigned long int time);
std::vector <double> norm (std::vector <unsigned long long>& d_of_time, unsigned long int W);
unsigned long int abs_threshold(std::vector <double>& normalized, unsigned long int W, float threshold);

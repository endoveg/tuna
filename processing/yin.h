#include "../audio/sampling.h"
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

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
  long int at(unsigned long int time, unsigned long int lag);
  ~acf() {
    //free(is_calculated);
    //free(value);
  }
 private:
  amplitude_probes * A;
  unsigned int window_size;
  short int  *is_calculated;
  unsigned long int *value;
};

double amplitude_probes::yin(float threshold, unsigned int window_size) {
  acf ACF(this, window_size);
  long int d;
  // t = 0, tau = 1
  //d = ACF.at(0,0) + ACF.at(1,0) - 2 * ACF.at(0,1);
  long int tau;
  for(tau = 0; tau <= window_size; tau++) {
    d = ACF.at(0,0) + ACF.at(tau,0) - 2 * ACF.at(0,tau);
    printf("%ld\n", d);
  }
  return 0.0;
}

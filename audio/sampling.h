#include <sys/ipc.h>
#include <sys/shm.h>
#include <pthread.h>
#include <inttypes.h>
#include <vector>

#ifndef SAMPLING_H
#define SAMPLING_H

class amplitude_probes {
 public:
  void *amplitudes;
  void capture(int num_for_key);
  pthread_mutex_t is_done;
  unsigned int rate;
  unsigned long int count;
  short unsigned int bits_per_sample;
  amplitude_probes(unsigned int r, unsigned long int c,
		   short unsigned int bps) {
    bits_per_sample = bps;
    rate = r;
    count = c;
    cold_start = true;
  }
  ~amplitude_probes();
  int operator[] (unsigned long int m) {
    /*
      There is also 24 float, but it's harder to "decode"
      1 byte = 8 bit
      2 bytes = 16 bit
     */
    switch (bits_per_sample) {
    case 1:
      return *((int8_t *)amplitudes + m);
    case 2:
      return *((int16_t *)amplitudes + m);
    }
  }
  void set(unsigned int index, long int value);
  double yin(float threshold, unsigned int window_size,
	     unsigned long int time);
  std::vector <double> d_of_time; //needed to step with linear
  bool cold_start;
  std::vector <double> filtered;
  double get(unsigned int index);
};

struct _sampling_thread_arg {
  char const* audio_stream;
  amplitude_probes * amp_params;
};

typedef struct _sampling_thread_arg sampling_thread_arg;
#endif

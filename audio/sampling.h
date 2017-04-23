#include <sys/ipc.h>
#include <sys/shm.h>
#include <pthread.h>
#include <inttypes.h>
#include <vector>
#include <unistd.h>
#include <stdio.h>

#ifndef SAMPLING_H
#define SAMPLING_H

class non_copyable
{
protected:
    non_copyable() = default;
    ~non_copyable() = default;

    non_copyable(non_copyable const &) = delete;
    void operator=(non_copyable const &x) = delete;
};

class amplitude_probes: public non_copyable {
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
    amplitudes = NULL;
  }
  ~amplitude_probes();
  int operator[] (unsigned long int m) {
    /*
      There is also 24 float, but it's harder to "decode"
      1 byte = 8 bit
      2 bytes = 16 bit
     */
    if (this == NULL) {
      printf("Error NULL[m] is undefined\n");
      _exit(1);
    }
    switch (bits_per_sample) {
    case 1:
      return *((int8_t *)amplitudes + m);
    case 2:
      return *((int16_t *)amplitudes + m);
    }
  }
  
  void set(unsigned int index, long int value);
  
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


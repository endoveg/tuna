#include <sys/ipc.h>
#include <sys/shm.h>
#include <pthread.h>
#include <inttypes.h>
#include <vector>
#include <unistd.h>
#include <stdio.h>
#include <alsa/asoundlib.h>

#ifndef SAMPLING_H
#define SAMPLING_H

class __non_copyable {
protected:
    __non_copyable() = default;
    ~__non_copyable() = default;

    __non_copyable(__non_copyable const &) = delete;
    void operator=(__non_copyable const &x) = delete;
};

class audio_handler {
 private:
  unsigned int rate;
  short unsigned int bits_per_sample;
  char const* audio_device;
 public:
  int read_size;
  snd_pcm_t *capture_handle;
  audio_handler(unsigned int f, short unsigned int bps, char const* ad) {
    audio_device = ad;
    bits_per_sample = bps;
    rate = f;
    read_size = 2048;
  }
  void open();
  void close();
};


class amplitude_probes: public __non_copyable {
 public:
  void *amplitudes;
  void capture(audio_handler& audio);
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


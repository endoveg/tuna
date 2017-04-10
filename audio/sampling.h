#include <sys/ipc.h>
#include <sys/shm.h>
#include <pthread.h>
#include <inttypes.h>

struct _syncmaster {
  pthread_mutex_t is_done;
  int shared_meme_id;
};
typedef struct _syncmaster syncmaster;

class amplitude_probes {
 private:
  void * amplitudes;
 public:
  syncmaster * capture(int num_for_key);
  unsigned int rate;
  unsigned long int count;
  short unsigned int bits_per_sample;
  amplitude_probes(unsigned int r, unsigned long int c,
		   short unsigned int bps) {
    bits_per_sample = bps;
    rate = r;
    count = c;
  }
  long int operator[] (unsigned long int m) {
    /*
      There is also 24 float, but it's harder to "decode"
      1 byte = 8 bit
      2 bytes = 16 bit
     */
    switch (bits_per_sample) {
    case 1:
      return (long int) *(((int8_t *)this->amplitudes) + m);
    case 2:
      return (long int) *(((int16_t *)this->amplitudes) + m);
    }
  }
};

struct _sampling_thread_arg {
  char const* audio_stream;
  amplitude_probes * amp_params;
  syncmaster * nsync;
};

typedef struct _sampling_thread_arg sampling_thread_arg;

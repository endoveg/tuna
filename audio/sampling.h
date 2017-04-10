#include <sys/ipc.h>
#include <sys/shm.h>
#include <pthread.h>

struct _syncmaster {
  pthread_mutex_t is_done;
  int shared_meme_id;
};
typedef struct _syncmaster syncmaster;

class amplitude_probes {
 public:
  syncmaster * capture(int num_for_key);
  unsigned int rate;
  short unsigned int bits_per_sample;
  unsigned int duration;
  char *amplitudes;
  amplitude_probes(unsigned int r, unsigned int d,
		   short unsigned int bps) {
    rate = r;
    duration = d;
    bits_per_sample = bps;
  }
  amplitude_probes(unsigned int r, unsigned long int count,
		   short unsigned int bps) {
    bits_per_sample = bps;
    duration = count / rate * 1000; // in ms
    rate = r;
  }
};

struct _sampling_thread_arg {
  char const* audio_stream;
  amplitude_probes * amp_params;
  syncmaster * nsync;
};

typedef struct _sampling_thread_arg sampling_thread_arg;

#include <vector>
class amplitude_probes {
 private:
  unsigned int rate;
  short unsigned int bits_per_sample;
  unsigned int duration;
  char *amplitudes;
  amplitude_probes(unsigned int r, unsigned int d,
		   short unsigned int bps) {
    rate = r;
    duration = d;
  }
  capture();
  /*
    returns id of shared memory, where vector of amplitudes lie.b
   */
}

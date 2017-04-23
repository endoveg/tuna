#include "../audio/sampling.h"
#include "yin.h"
#include <vector>

extern double last_found_freq;
/*
  If tuner loses current frequency, it remains on
  last_found_freq.
 */

double acf::at(unsigned int time, unsigned  int lag) {
#if defined RECURSIVE
  if (time == 0) {
    double sum = 0;
    for (unsigned int j = time + 1; j <= time + window_size; j++) {
      sum += A.get(j) * A.get(j+lag);
    }
    return sum;
  }
  else {
    double v;
    v = this->at(time, lag) - A.get(time) * A.get(time + lag) +
      A.get(window_size+time+lag)+A.get(window_size+time);
    return v;
    }
#else 
  double sum = 0;
  unsigned int j = 0;
  for (j=time + 0; j <= window_size + time; j++) {
    sum += A->get(j) * A->get(j+lag);
  }
  return sum;
#endif
}

std::vector <double> diff(amplitude_probes& A, unsigned  int time, acf& ACF) {
  std::vector <double> d_t;
  unsigned int W = ACF.get_size_of_window();
  unsigned int tau;
  double t1, t2, t3;
  d_t.reserve(W+1);
  d_t.push_back(0);
  t1 = ACF.at(time,0);
  t2 = t1;
  for (tau = 1; tau <= W; tau++) {
    t3 = ACF.at(time,tau);
    t2 += A.get(time+tau+W)*A.get(time+tau+W) - A.get(time+tau)*A.get(time+tau);
    d_t.push_back(t1+t2 - 2*t3);
  }
  return d_t;
}

double yin(amplitude_probes &amp, float threshold, unsigned int W,
			     unsigned int time) {
  std::vector <double> normalized;
  if (amp.cold_start == true) {
    acf ACF(amp, W); //step 1
    amp.d_of_time = diff(amp, time, ACF); //step 2
    amp.cold_start = false;
  } else {
    double t1, t2, t3;
    t1 = amp.get(time+W)*amp.get(time+W) - amp.get(time)*amp.get(time); 
    for(unsigned int tau = 1; tau < W; tau++) {
      t2 = amp.get(time+tau+W)*amp.get(time+tau+W) - amp.get(time+tau)*amp.get(time+tau);
      t3 = amp.get(time+tau+W)*amp.get(time+W) - amp.get(time) * amp.get(time+tau);
      amp.d_of_time[tau] += t1 + t2 - 2 * t3;
    }
  }
  normalized = norm(amp.d_of_time, W); //step 3

  #if defined DIFF_DEBUG
  unsigned int tau_d;
  for (tau_d = 0; tau_d < W; tau_d++) {
    printf("%.4f\n", normalized[tau_d]);
  }
  #endif

  unsigned int tau = abs_threshold(normalized, W, threshold); //step 4
  if (tau == 0) {
    return -1;
  } else {
    return amp.rate / (double) quadratic_interpolation(amp, tau, W); //step 5
  }
}

std::vector <double> norm(std::vector <double>& d_of_time, unsigned  int W) {
  d_of_time[0] = 1.0;
  double sum_for_norm = 0;
  unsigned  int tau;
  std::vector <double> normalized;
  normalized.reserve(W);
  for(tau = 1; tau <= W; tau++) {
    sum_for_norm += d_of_time[tau];
    normalized.push_back((double) d_of_time[tau] * (double) tau / (double) sum_for_norm);
  }
  return normalized;
}

unsigned long int abs_threshold(std::vector <double>& normalized, unsigned  int W, float threshold) {
  unsigned int tau = 0;
  bool is_found = false;
  while (tau <= W && !is_found){
    while (normalized[tau] <= threshold ){
      if ((tau + 1 <= W) &&
	  (normalized[tau]) >= normalized[tau+1]) {
	tau++;
      } else {
	is_found = true;
	break;
      }
    }
    tau++;
  }
  if (tau == W+1) {
    return 0;
  } else {
    return tau;
  }
}


void yin_tuner(amplitude_probes& amp, float threshold, unsigned int window_size) {
  double cur_freq = yin(amp, threshold, window_size,0);
  if (cur_freq <= 75.0) {
    printf("%.1f\n", last_found_freq);
  } else {
    printf("%.1f\n", cur_freq);
  }
  for (unsigned int i = 1; i < window_size; i++) {
    cur_freq =  yin(amp, threshold, window_size,i);
    if (cur_freq <= 75.0) {
      /*
	Lower frequencies are considered to be noise or 
	non harmonic sound. (yin_tune gives -1)
       */
      printf("%.1f\n", last_found_freq);
    } else {
      printf("%.1f\n", cur_freq);
      last_found_freq = cur_freq;
    }
 }
}

double quadratic_interpolation(amplitude_probes &amp, unsigned int x1, unsigned int W) {
  /*
    See https://ccrma.stanford.edu/~jos/sasp/Quadratic_Interpolation_Spectral_Peaks.html
   */
  
  unsigned int x0, x2;
  double better;
  if (x1 < 1) {
    x0 = x1;
  } else {
    x0 = x1-1;
  }
  if (x1 + 1 < W) {
    x2 = x1 + 1;
  } 
  else {
    x2 = x1;
  }
  if (x0 == x1) {
    if (amp.d_of_time[x1] <= amp.d_of_time[x2]) {
      better = x1;
    } 
    else {
      better = x2;
    }
  } else if (x2 == x1) {
    if (amp.d_of_time[x1] <= amp.d_of_time[x0]) {
      better = x1;
    } 
    else {
      better = x0;
    }
  }
  else {
    double s0, s1, s2;
    s0 = amp.d_of_time[x0];
    s1 = amp.d_of_time[x1];
    s2 = amp.d_of_time[x2];
    better = x1 + (s2 - s0) / (2 * (2 * s1 - s2 - s0));
  }
  return better;
}

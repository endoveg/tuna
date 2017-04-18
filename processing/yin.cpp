#include "../audio/sampling.h"
#include "yin.h"
#include <vector>

long long acf::at(unsigned long int time, unsigned long int lag) {
  if (time == 0) {
    long long sum = 0;
    for (int j = time + 1; j <= time + window_size; j++) {
      sum += (*A)[j] * (*A)[j+lag];
    }
    return sum;
  }
  else {
    long long v;
    v = this->at(time - 1, lag) -
      (*A)[time] * (*A)[time+lag] +
      (*A)[window_size+time+lag] * (*A)[window_size+time];
    return v;
    }
}

std::vector <unsigned long long> diff(amplitude_probes& A, unsigned long int time, acf& ACF) {
  std::vector <unsigned long long> d_t;
  unsigned int W = ACF.get_size_of_window();
  unsigned long int tau;
  unsigned long long t1, t2, t3;
  d_t.reserve(W+1);
  d_t.push_back(0);
  t1 = ACF.at(time,0);
  t2 = t1;
  for (tau = 1; tau <= W; tau++) {
    t3 = ACF.at(time,tau);
    t2 += A[time+tau+W]*A[time+tau+W] - A[time+tau]*A[time+tau];
    d_t.push_back(t1+t2 - 2*t3);
  }
  return d_t;
}

double amplitude_probes::yin(float threshold, unsigned int window_size,
			     unsigned long int time) {
   acf ACF(this, window_size); //step 1
  std::vector <unsigned long long> d_of_time = diff(*this, time, ACF); //step 2
  std::vector <double> normalized = norm(d_of_time, window_size); //step 3
  //  debug step
  /* unsigned long long tau_d;
       for (tau_d = 0; tau_d < window_size; tau_d++) {
      printf("%.4f\n", normalized[tau_d]);
      }*/
  unsigned long int tau = abs_threshold(normalized, window_size, threshold);
  if (tau == 0) {
    return -1;
  } else {
    return rate / (double) tau;
  }
}

std::vector <double> norm(std::vector <unsigned long long>& d_of_time, unsigned long int W) {
  d_of_time[0] = 1;
  unsigned long long sum_for_norm = 0;
  unsigned long int tau;
  std::vector <double> normalized;
  normalized.reserve(W);
  for(tau = 1; tau <= W; tau++) {
    sum_for_norm += d_of_time[tau];
    normalized.push_back((double) d_of_time[tau] * (double) tau / (double) sum_for_norm);
  }
  return normalized;
}

unsigned long int abs_threshold(std::vector <double>& normalized, unsigned long int W, float threshold) {
  unsigned long int tau = 0;
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
  if (tau == W) {
    return 0;
  } else {
    //    printf("found:%lu\n", tau);
    return tau;
  }
}

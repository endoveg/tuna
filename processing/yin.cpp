#include "../audio/sampling.h"
#include "yin.h"
#include <vector>

long long acf::at(unsigned long int time, unsigned long int lag) {
  if (time == 0) {
    long long sum = 0;
    for (int j = time + 1; j <= time + window_size; j++) {
      sum += (*A)[j] * (*A)[j+lag];
    }
    //*(value + time + lag * window_size) = sum;
    //*(is_calculated + time + lag * window_size) = 1;
    return sum;
  }
  //assert time == 0 || time == 1
  else //if (b_index(time, lag) == 0) {
    {
    long long v;
    v = this->at(time - 1, lag) -
      (*A)[time] * (*A)[time+lag] +
      (*A)[window_size+time-lag-1] * (*A)[window_size+time-1];
    //   *(value + time + lag * window_size) = v;
    //    *(is_calculated + time + lag * window_size) = 1;
    return v;
    }
  //} else {
  // return index(time, lag);
  // }
}

std::vector <unsigned long long> diff(acf& ACF, unsigned long int time) {
  std::vector <unsigned long long> d_t;
  unsigned int W = ACF.get_size_of_window();
  unsigned long int tau;
  d_t.reserve(W);
  for (tau = time+1; tau <= time + W; tau++) {
    d_t.push_back(ACF.at(time, 0) + ACF.at(tau+time, 0) - 2 *
		 ACF.at(time, tau));
  }
  return d_t;
}

double amplitude_probes::yin(float threshold, unsigned int window_size,
			     unsigned long int time) {
  acf ACF(this, window_size); //step 1
  std::vector <unsigned long long> d_of_time = diff(ACF, time); //step 2
  std::vector <double> normalized = norm(d_of_time, window_size); //step 3
  //  debug step
    unsigned long long tau_d;
    for (tau_d = 0; tau_d < window_size; tau_d++) {
      printf("%.4f\n", normalized[tau_d]);
    }
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

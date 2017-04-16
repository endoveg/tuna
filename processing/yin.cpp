#include "../audio/sampling.h"
#include "yin.h"

long int acf::at(unsigned long int time, unsigned long int lag) {
  if (time == 0) {
    long int sum = 0;
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
    long int v;
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

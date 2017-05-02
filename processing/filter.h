#include <iostream>
#include <vector>
#include <fstream>
#include "../audio/sampling.h"
#ifndef FILTER_H
#define FILTER_H

class biquad {
 private:
  /*
    H(z) = (b0 + b1*z^-1 + b2*z^-2) : (a0 + a1*z^-1 + a2*z^-2)
    a0 = 1
   */
  double b1,b2,b0; 
  double a1,a2,a0; 
  double z1, z2; //temp delay registers
  double process(double in);
 public:
  void apply(std::vector <double> x);
  void apply(amplitude_probes *amp);
  biquad (std::ifstream& file);
};


class filter {
 private:
  std::vector <biquad> cascades;
  int k; //number of biquad cascades;
 public:
  filter (std::ifstream& file);
  void apply(amplitude_probes* amp);
};



#endif

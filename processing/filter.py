#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Sun Apr 23 17:44:21 2017

@author: egor
"""
from scipy.signal import filter_design as fd

# Specification for our filter
F_NY = 44100/2
Fp = 82
Fs = 50
Wp = Fp/F_NY # end of passband, normalized frequency
Ws = Fs/F_NY # start of the stopband, normalized frequency 
Rp = 1     # passband maximum loss (gpass)
As = 120      # stoppand min attenuation (gstop)

# The ellip filter design
M = fd.iirdesign(Wp, Ws, Rp, As, ftype='ellip', output = 'sos')
k = len(M)
# Print coef to file
f = open("coef", "w")
f.write("{}\n".format(k))
for coef in M:
        b0,b1,b2,a0,a1,a2 = coef
        s = "{} {} {} {} {} {}\n".format(b0,b1,b2,a0,a1,a2)
        f.write(s)
f.close()

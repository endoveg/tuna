#include <complex>
#include <cmath>
#include <vector>
#include <iostream>
#include <pthread.h>
#include "dft.h"
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include "../audio/sampling.h"

unsigned int bit_reverse (unsigned int x, unsigned int size)
{
    unsigned int result = 0;
    unsigned int lg = log2 ((double) size);
    while (lg > 0)
    {
        result = result << 1;
        result = result + (x & 1);
        x = x >> 1;
        lg--;
    }
    return result;
}

// The algoruthm requires input data to be index bit-reversed. 
// That is, for example, if one has an array of 8 elemants, the input data order should be
// a[0], a[4], a[2], a[6], a[1], a[3], a[5], a[7]
 
void permutate_array (std::vector <std::complex <double> >& raw, const unsigned int size)
{
    unsigned int j = 0;
    std::complex <double> tmp;
    for (unsigned int i = 0; i < size; i++)
    {
        j = bit_reverse (i, size);
        if (j > i)
        {
            tmp = raw [i];
            raw [i] = raw [j];
            raw [j] = tmp;
        }
    }
}
// Cooley-Tukey radix-2 algorithm
void fft(amplitude_probes& proto, std::vector <std::complex <double> > &image)
{
    const unsigned int sample_nu = proto.count;
    // checking whether sample_nu is a power of 2
    // Maybe it's beter to throw an exclusion here
    if ((sample_nu == 0) || (sample_nu & (sample_nu - 1) != 0))
    {
        std::cout << "Size of the array is not a power of 2!" << std::endl;
        return;
    }
    unsigned int lgn = log2 (sample_nu);
    for (unsigned int i = 0; i < sample_nu; i++)
        image [i] = std::complex <double> (proto [i]);
    permutate_array (image, sample_nu);
    unsigned int m_size = 0;
    unsigned int m_half = 0;
    std::complex <double> twiddle;
    std::complex <double> tmp0;
    std::complex <double> tmp1;
    // at the first step the permutated values are used in pairs as they go in an array
    // to compute size-2 DFTs. This idea is implemented recursevly
    for (unsigned int lgm = 1; lgm <= lgn; lgm++)
    {
        m_size = pow (2, lgm);
        m_half = m_size / 2;
        for (unsigned int j = 0; j < m_half; j++)
        {
            twiddle = exp (std::complex <double> (0, -2 * M_PI * j / m_size));
            for (unsigned int i = 0; i <= sample_nu - m_size; i += m_size)
            {
                tmp0 = std::complex <double> (image [i + j]);
                tmp1 = std::complex <double> (image [i + j + m_half]) * twiddle;
                image [i + j] = tmp0 + tmp1;
                image [i + j + m_half] = tmp0 - tmp1;
            }
        }
    }
}

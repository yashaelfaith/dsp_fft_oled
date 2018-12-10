#ifndef FIXFFT_H
#define FIXFFT_H

#include <stdint.h>

short isqrt(short num);
int8_t FIX_MPY(int8_t a, int8_t b);
int fix_fft(int8_t fr[], int8_t fi[], int m, int inverse);

#endif
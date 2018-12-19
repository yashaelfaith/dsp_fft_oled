// #include <avr/pgmspace.h>
#include "fix_fft.h"
// #include <WProgram.h>

/* fix_fft.c - Fixed-point in-place Fast Fourier Transform  */
/*
  All data are fixed-point short integers, in which -32768
  to +32768 represent -1.0 to +1.0 respectively. Integer
  arithmetic is used for speed, instead of the more natural
  floating-point.

  For the forward FFT (time -> freq), fixed scaling is
  performed to prevent arithmetic overflow, and to map a 0dB
  sine/cosine wave (i.e. amplitude = 32767) to two -6dB freq
  coefficients. The return value is always 0.

  For the inverse FFT (freq -> time), fixed scaling cannot be
  done, as two 0dB coefficients would sum to a peak amplitude
  of 64K, overflowing the 32k range of the fixed-point integers.
  Thus, the fix_fft() routine performs variable scaling, and
  returns a value which is the number of bits LEFT by which
  the output must be shifted to get the actual amplitude
  (i.e. if fix_fft() returns 3, each value of fr[] and fi[]
  must be multiplied by 8 (2**3) for proper scaling.
  Clearly, this cannot be done within fixed-point short
  integers. In practice, if the result is to be used as a
  filter, the scale_shift can usually be ignored, as the
  result will be approximately correctly normalized as is.

  Written by:  Tom Roberts  11/8/89
  Made portable:  Malcolm Slaney 12/15/94 malcolm@interval.com
  Enhanced:  Dimitrios P. Bouras  14 Jun 2006 dbouras@ieee.org
  Modified for 8bit values David Keller  10.10.2010
*/


#define N_WAVE  256    /* full length of Sinewave[] */
#define LOG2_N_WAVE 8   /* log2(N_WAVE) */




/*
  Since we only use 3/4 of N_WAVE, we define only
  this many samples, in order to conserve data space.
*/



const int8_t Sinewave[N_WAVE-N_WAVE/4] = {
0, 3, 6, 9, 12, 15, 18, 21,
24, 28, 31, 34, 37, 40, 43, 46,
48, 51, 54, 57, 60, 63, 65, 68,
71, 73, 76, 78, 81, 83, 85, 88,
90, 92, 94, 96, 98, 100, 102, 104,
106, 108, 109, 111, 112, 114, 115, 117,
118, 119, 120, 121, 122, 123, 124, 124,
125, 126, 126, 127, 127, 127, 127, 127,

127, 127, 127, 127, 127, 127, 126, 126,
125, 124, 124, 123, 122, 121, 120, 119,
118, 117, 115, 114, 112, 111, 109, 108,
106, 104, 102, 100, 98, 96, 94, 92,
90, 88, 85, 83, 81, 78, 76, 73,
71, 68, 65, 63, 60, 57, 54, 51,
48, 46, 43, 40, 37, 34, 31, 28,
24, 21, 18, 15, 12, 9, 6, 3,

0, -3, -6, -9, -12, -15, -18, -21,
-24, -28, -31, -34, -37, -40, -43, -46,
-48, -51, -54, -57, -60, -63, -65, -68,
-71, -73, -76, -78, -81, -83, -85, -88,
-90, -92, -94, -96, -98, -100, -102, -104,
-106, -108, -109, -111, -112, -114, -115, -117,
-118, -119, -120, -121, -122, -123, -124, -124,
-125, -126, -126, -127, -127, -127, -127, -127,

/*-127, -127, -127, -127, -127, -127, -126, -126,
-125, -124, -124, -123, -122, -121, -120, -119,
-118, -117, -115, -114, -112, -111, -109, -108,
-106, -104, -102, -100, -98, -96, -94, -92,
-90, -88, -85, -83, -81, -78, -76, -73,
-71, -68, -65, -63, -60, -57, -54, -51,
-48, -46, -43, -40, -37, -34, -31, -28,
-24, -21, -18, -15, -12, -9, -6, -3, */
};






/*
  FIX_MPY() - fixed-point multiplication & scaling.
  Substitute inline assembly for hardware-specific
  optimization suited to a particluar DSP processor.
  Scaling ensures that result remains 16-bit.
*/
int8_t FIX_MPY(int8_t a, int8_t b)
{
  
  //Serial.println(a);
 //Serial.println(b);
  
  
    /* shift right one less bit (i.e. 15-1) */
    int c = ((int)a * (int)b) >> 6;
    /* last bit shifted out = rounding-bit */
    b = c & 0x01;
    /* last shift + rounding bit */
    a = (c >> 1) + b;

      /*
      Serial.println(Sinewave[3]);
      Serial.println(c);
      Serial.println(a);
      while(1);*/

    return a;
}

/*
  fix_fft() - perform forward/inverse fast Fourier transform.
  fr[n],fi[n] are real and imaginary arrays, both INPUT AND
  RESULT (in-place FFT), with 0 <= n < 2**m; set inverse to
  0 for forward transform (FFT), or 1 for iFFT.
*/
int fix_fft(int8_t fr[], int8_t fi[], int m, int inverse)
{
    int mr, nn, i, j, l, k, istep, n, scale, shift;
    int8_t qr, qi, tr, ti, wr, wi;

    n = 1 << m;

    /* max FFT size = N_WAVE */
    if (n > N_WAVE)
      return -1;

    mr = 0;
    nn = n - 1;
    scale = 0;

    /* decimation in time - re-order data */
    for (m=1; m<=nn; ++m) {
      l = n;
      do {
        l >>= 1;
      } while (mr+l > nn);
      mr = (mr & (l-1)) + l;

      if (mr <= m)
        continue;
      tr = fr[m];
      fr[m] = fr[mr];
      fr[mr] = tr;
      ti = fi[m];
      fi[m] = fi[mr];
      fi[mr] = ti;
    }

    l = 1;
    k = LOG2_N_WAVE-1;
    while (l < n) {
      if (inverse) {
        /* variable scaling, depending upon data */
        shift = 0;
        for (i=0; i<n; ++i) {
            j = fr[i];
            if (j < 0)
              j = -j;
            m = fi[i];
            if (m < 0)
              m = -m;
            if (j > 16383 || m > 16383) {
              shift = 1;
              break;
            }
        }
        if (shift)
            ++scale;
      } else {
        /*
          fixed scaling, for proper normalization --
          there will be log2(n) passes, so this results
          in an overall factor of 1/n, distributed to
          maximize arithmetic accuracy.
        */
        shift = 1;
      }
      /*
        it may not be obvious, but the shift will be
        performed on each data point exactly once,
        during this pass.
      */
      istep = l << 1;
      for (m=0; m<l; ++m) {
        j = m << k;
        /* 0 <= j < N_WAVE/2 */
        wr =  Sinewave[j+N_WAVE/4];

/*Serial.println("asdfasdf");
Serial.println(wr);
Serial.println(j+N_WAVE/4);
Serial.println(Sinewave[256]);

Serial.println("");*/


        wi = -Sinewave[j];
        if (inverse)
            wi = -wi;
        if (shift) {
            wr >>= 1;
            wi >>= 1;
        }
        for (i=m; i<n; i+=istep) {
            j = i + l;
            tr = FIX_MPY(wr,fr[j]) - FIX_MPY(wi,fi[j]);
            ti = FIX_MPY(wr,fi[j]) + FIX_MPY(wi,fr[j]);
            qr = fr[i];
            qi = fi[i];
            if (shift) {
              qr >>= 1;
              qi >>= 1;
            }
            fr[j] = qr - tr;
            fi[j] = qi - ti;
            fr[i] = qr + tr;
            fi[i] = qi + ti;
        }
      }
      --k;
      l = istep;
    }
    return scale;
}

/*
  fix_fftr() - forward/inverse FFT on array of real numbers.
  Real FFT/iFFT using half-size complex FFT by distributing
  even/odd samples into real/imaginary arrays respectively.
  In order to save data space (i.e. to avoid two arrays, one
  for real, one for imaginary samples), we proceed in the
  following two steps: a) samples are rearranged in the real
  array so that all even samples are in places 0-(N/2-1) and
  all imaginary samples in places (N/2)-(N-1), and b) fix_fft
  is called with fr and fi pointing to index 0 and index N/2
  respectively in the original array. The above guarantees
  that fix_fft "sees" consecutive real samples as alternating
  real and imaginary samples in the complex array.
*/
int fix_fftr(int8_t f[], int m, int inverse)
{
    int i, N = 1<<(m-1), scale = 0;
    int8_t tt, *fr=f, *fi=&f[N];

    if (inverse)
      scale = fix_fft(fi, fr, m-1, inverse);
    for (i=1; i<N; i+=2) {
      tt = f[N+i-1];
      f[N+i-1] = f[i];
      f[i] = tt;
    }
    if (! inverse)
      scale = fix_fft(fi, fr, m-1, inverse);
    return scale;
} 

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

short isqrt(short num);

int main () {

    int8_t data[128] = {
    // 8 bit Resolution
    //    0,  39,  74, 102, 120, 127, 120, 102,  74,  39,   0, -40, -75,
    // -103,-121,-127,-121,-103, -75, -40,  -1,  39,  74, 102, 120, 127,
    //  120, 102,  74,  39,   0, -40, -75,-103,-121,-127,-121,-103, -75,
    //  -40,  -1,  39,  74, 102, 120, 127, 120, 102,  74,  39,   0, -40,
    //  -75,-103,-121,-127,-121,-103, -75, -40,  -1,  39,  74, 102, 120,
    //  127, 120, 102,  74,  39,   0, -40, -75,-103,-121,-127,-121,-103,
    //  -75, -40,  -1,  39,  74, 102, 120, 127, 120, 102,  74,  39,   0,
    //  -40, -75,-103,-121,-127,-121,-103, -75, -40,  -1,  39,  74, 102,
    //  120, 127, 120, 102,  74,  39,   0, -40, -75,-103,-121,-127,-121,
    // -103, -75, -40,  -1,  39,  74, 102, 120, 127, 120, 102

     //  0,  59, 105, 126, 117,  80,  24, -37, -90,-122,-125, -99, -49,
     // 12,  70, 112, 127, 112,  70,  12, -49, -99,-125,-122, -90, -37,
     // 24,  80, 117, 126, 105,  59,  -1, -60,-106,-127,-118, -81, -25,
     // 36,  89, 121, 124,  98,  48, -13, -71,-113,-127,-113, -71, -13,
     // 48,  98, 124, 121,  89,  36, -25, -81,-118,-127,-106, -60,   0,
     // 59, 105, 126, 117,  80,  24, -37, -90,-122,-125, -99, -49,  12,
     // 70, 112, 127, 112,  70,  12, -49, -99,-125,-122, -90, -37,  24,
     // 80, 117, 126, 105,  59,  -1, -60,-106,-127,-118, -81, -25,  36,
     // 89, 121, 124,  98,  48, -13, -71,-113,-127,-113, -71, -13,  48,
     // 98, 124, 121,  89,  36, -25, -81,-118,-127,-106, -60

      0, 0, 0, 0, 0, 0, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
      127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 127, 127, 127, 127, 127,
      127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
      127, 127, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
      127, 127, 127, 127, 127, 127, 127, 127, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0
    };
    float res[128];
    int8_t im[128];//, data_avgs[8];
    int i;//, j, x, y;

    struct timeval t1, t2, t3;
    double elapsedTime;

    gettimeofday(&t1, NULL);

    for (i = 0; i < 128; i++) {
      // data[i] = 0;
      im[i] = 0;
    }
    fix_fft(data, im, 7, 0);  // FFT processing

    // printf("data\n");
    // for (i = 0; i < 128; i++) {
    //     printf("%d, ", data[i]);
    //     printf("%d\n", im[i]);
    // }

    gettimeofday(&t2, NULL);

    for (i = 0; i < 64; i++){
        // data[i] = (uint8_t) 2*isqrt(data[i] * data[i] + im[i] * im[i]); // Make values positive
        res[i] = (float) 2*isqrt(data[i] * data[i] + im[i] * im[i])/128.0; 
        // data[i] = 2*sqrt(data[i] * data[i] + im[i] * im[i]); // Make values positive
    }

    gettimeofday(&t3, NULL);

    printf("averaged\n");
    for (i = 0; i < 64; i++)
        printf("%f\n", res[i]);

    elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000000.0;      // sec to ms
    elapsedTime += (t2.tv_usec - t1.tv_usec) ;   // us to ms
    printf("elapsed time fft: %lf us\n", elapsedTime);

    elapsedTime = (t3.tv_sec - t2.tv_sec) * 1000000.0;      // sec to ms
    elapsedTime += (t3.tv_usec - t2.tv_usec) ;   // us to ms
    printf("elapsed time absolute: %lf us\n", elapsedTime);

    // printf("t1:%d, t2:%d, t3:%d\n", t1.tv_sec,t2.tv_sec,t3.tv_sec);
    // printf("t1:%d, t2:%d, t3:%d\n", t1.tv_usec,t2.tv_usec,t3.tv_usec);
    return 0;
}


short isqrt(short num) {
    short res = 0;
    short bit = 1 << 14; // The second-to-top bit is set: 1 << 30 for 32 bits
 
    // "bit" starts at the highest power of four <= the argument.
    while (bit > num)
        bit >>= 2;
        
    while (bit != 0) {
        if (num >= res + bit) {
            num -= res + bit;
            res += bit << 1;
        }
        
        res >>= 1;
        bit >>= 2;
    }
    return res;
}
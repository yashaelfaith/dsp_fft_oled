// Include Libraries
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>

#define N_WAVE  256    /* full length of Sinewave[] */
#define LOG2_N_WAVE 8   /* log2(N_WAVE) */

// Global Variable
const double PI = 3.141592653589793238460;

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



// Function Prototypes
short isqrt(short num);
int8_t FIX_MPY(int8_t a, int8_t b);
int fix_fft(int8_t fr[], int8_t fi[], int m, int inverse);


int app_main()
{
    int i;
    // cplx input[8] = {1,3,3,1,0,0,0,0};
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

      0,  59, 105, 126, 117,  80,  24, -37, -90,-122,-125, -99, -49,
     12,  70, 112, 127, 112,  70,  12, -49, -99,-125,-122, -90, -37,
     24,  80, 117, 126, 105,  59,  -1, -60,-106,-127,-118, -81, -25,
     36,  89, 121, 124,  98,  48, -13, -71,-113,-127,-113, -71, -13,
     48,  98, 124, 121,  89,  36, -25, -81,-118,-127,-106, -60,   0,
     59, 105, 126, 117,  80,  24, -37, -90,-122,-125, -99, -49,  12,
     70, 112, 127, 112,  70,  12, -49, -99,-125,-122, -90, -37,  24,
     80, 117, 126, 105,  59,  -1, -60,-106,-127,-118, -81, -25,  36,
     89, 121, 124,  98,  48, -13, -71,-113,-127,-113, -71, -13,  48,
     98, 124, 121,  89,  36, -25, -81,-118,-127,-106, -60
    };
    int8_t im[128];
    struct timeval t1, t2, t3;
    double elapsedTime;

    gettimeofday(&t1, NULL);
    for (i = 0; i < 128; i++) {
        im[i] = 0;
    }

    // printf("Before FFT: \n");
    // for (i = 0; i < 8; i++){
    //     printf("%f  %fi\n", creal(input[i]), cimag(input[i]));
    // }

    short result[64];

    // fft(input, 8);
    fix_fft(data, im, 7, 0);
    gettimeofday(&t2, NULL);
    for (i = 0; i < 64; i++) {
        result[i] = 2*isqrt(data[i] * data[i] + im[i] * im[i]);
    }
    gettimeofday(&t3, NULL);

    printf("After FFT: \n");
    for (i = 0; i < 64; i++){
        // printf("%f  %fi\n", creal(input[i]), cimag(input[i]));
        printf("%d\n", result[i]);
    }

    elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000000.0;      // sec to ms
    elapsedTime += (t2.tv_usec - t1.tv_usec) ;   // us to ms
    printf("elapsed time fft: %lf us\n", elapsedTime);

    elapsedTime = (t3.tv_sec - t2.tv_sec) * 1000000.0;      // sec to ms
    elapsedTime += (t3.tv_usec - t2.tv_usec) ;   // us to ms
    printf("elapsed time absolute: %lf us\n", elapsedTime);

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
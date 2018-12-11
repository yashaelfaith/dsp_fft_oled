#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>

#define BUFFERLENGTH 33
#define LENGTH 200

// int buffer_offset = 0;
// int buffer_int[BUFFERLENGTH];
int x_int[LENGTH];
int y_int[LENGTH];


/*=============================================================*/
/*                       Koefisien Filter                      */
/*=============================================================*/
/*   Koefisien Filter merupakan bilangan fraksional dengan     */
/*   pengali bernilai 2 pangkat 15.                            */
/*   Diperoleh dari MATLAB dengan menggunakan fungsi fir1().   */
/*=============================================================*/

// int koef_filter_int[BUFFERLENGTH] = {
// -1, 1, 58, -3, -1, 13, 18, -1, 2, -28, -3, -1, -1, -75, -1, -1,
// 5, -1, 1, 2, 1, -3, -1, 1, -24, 1, -1, 17, -1, 1, 1, 40, 4
// };

int8_t *koef_filter_int;

// LOW PASS FILTER
int8_t koef_lpf1[BUFFERLENGTH] = {
-1, 1, 58, -3, -1, 13, 18, -1, 2, -28, -3, -1, -1, -75, -1, -1,
5, -1, 1, 2, 1, -3, -1, 1, -24, 1, -1, 17, -1, 1, 1, 40, 4
};

int8_t koef_lpf2[BUFFERLENGTH] = {
-1, -2, 65, -4, -1, 1, -10, 1, 1, 24, -7, 1, -3, 161, -5, 1, 7,
-18, -2, -2, -33, 10, 1, -1, 64, 3, 1, -1, 43, 3, 3, -43, -1
};

int8_t koef_lpf3[BUFFERLENGTH] = {
-1, -3, 58, 1, -1, -8, -28, 2, -1, -18, -12, 1, -3, -75, -1, -1,
-7, 40, 2, 1, -36, 23, 1, 5, -24, 1, -1, -4, 40, 1, 1, -1, -17
};

int8_t koef_lpf4[BUFFERLENGTH] = {
-1, -2, 40, 7, -1, -1, 75, -3, 1, 12, -18, 1, -2, -28, 8, -1, -1,
-58, -3, -1, -17, -1, -1, -1, -40, -4, -1, -1, -24, -5, -1, 23, 36
};

int8_t koef_lpf5[BUFFERLENGTH] = {
-2, -2, 18, 7, -1, 5, 161, 3, -1, -7, -24, -1, -1, 10, -1, 1, 4,
65, 2, 1, -1, -43, -3, -3, 43, 1, 1, 3, 193, 1, 1, -10, -33
};

int8_t koef_lpf6[BUFFERLENGTH] = {
-2, -1, 1, -5, 1, 1, 75, -1, -1, 3, -28, -2, 1, 18, -13, -1, -3,
-58, -1, -1, 4, -40, -1, -1, 1, 17, -1, -1, -24, -1, -1, -3, -1
};

int8_t koef_lpf7[BUFFERLENGTH] = {
-3, -1, -10, -18, 1, -3, -28, -6, -1, 1, -31, -3, 1, 5, -1, -1,
-1, 40, -3, -1, 1, 24, 5, 1, -23, -36, -2, -3, 40, 1, -1, -1, 44
};

// HIGH PASS FILTER
int8_t koef_hpf1[BUFFERLENGTH] = {
-2, -1, -10, -10, -1, -1, -10, 10, -1, -2, 225, -3, 1, -7, 26, 2,
2, -18, 7, -1, -3, 64, -1, 1, 10, 33, -3, -3, 43, 14, -1, 2, 193
};

int8_t koef_hpf2[BUFFERLENGTH] = {
1, -1, -6, 28, -3, 1, 18, -10, 1, 3, -31, -1, -1, -7, -1, -1, -1,
-1, -12, 1, -1, 24, 1, -1, 3, -1, -3, -2, -1, 0, -1, 3, 44
};

int8_t koef_hpf3[BUFFERLENGTH] = {
3, -1, -1, 75, 1, 1, -5, 1, -1, -2, -28, 6, -1, -1, -81, -3, -1,
10, 18, 2, 3, -40, -1, -1, -1, -44, -1, -1, -23, -36, 2, -4, -1
};

int8_t koef_hpf4[BUFFERLENGTH] = {
7, -1, 3, 97, 5, -1, -7, 18, 2, 2, -24, 10, 1, 4, 129, 4, 1,-10,
-24, 3, 3, -43, -14, 1, -2, 64, 3, 1, -10, 1, -1, -1, -33
};

int8_t koef_hpf5[BUFFERLENGTH] = {
12, -1, 3, 75, -1, -1, 7, -40, -2, -1, -18, 10, 1, 3, -81, -1,
-1, 6, 28, 3, 2, -1, 1, 1, -3, -44, -1, -1, 3, 56, -4, 2, 36

};

int8_t koef_hpf6[BUFFERLENGTH] = {
18, -1, 2, 28, -8, -1, 1, 58, 3, 1, -12, -1, -1, -1, -1, -7, -1,
-1, -31, -1, 1, 23, 36, -2, 4, -1, -4, -1, -1, 1, 3, -1, -17

};

int8_t koef_hpf7[BUFFERLENGTH] = {
24, 1, 1, -10, 1, -1, -4, 193, -2, -1, -7, -18, -2, -2, 26, 7, 1,
-3, 32, -3, -1, 10, -1, 1, 1, 33, -1, 1, -2, 193, 2, -1, -1
};

// BAND PASS FILTER
int8_t koef_bpf1[BUFFERLENGTH] = {
28, 2, -1, -18, 13, 1, 3, 58, 1, -1, -3, -40, -1, -1, -1, 5, -1,
3, -31, 1, -1, -3, -56, 4, -2, -36, 17, -1, 3, 1, -1, -1, 4
};

int8_t koef_bpf2[BUFFERLENGTH] = {
31, 3, -1, -5, -1, 1, 1, -40, 3, -1, -1, -58, 3, 1, -13, -18, 1,
-2, 28, 4, -1, -1, 0, -3, 1, 17, 36, -2, 4, 56, -3, -1, -1
};

int8_t koef_bpf3[BUFFERLENGTH] = {
32, 3, -1, 7, -26, -2, -2, 18, -7, 1, 2, 193, 4, 1, -1, 10, -1,
-1, -24, -1, -1, 2, 64, -2, 1, 1, 33, -1, -1, 1, -10, -1, -3
};

int8_t koef_bpf4[BUFFERLENGTH] = {
31, -1, -1, 7, 1, 1, 1, 1, 12, 1, 3, -58, -1, -1, 8, 28, -2, 1,
18, -17, 1, -3, 0, -1, -1, -4, -1, 4, -2, -36, 23, 1, -1
};

int8_t koef_bpf5[BUFFERLENGTH] = {
28, -6, -1, 1, 81, 3, 1, -10, -18, 1, 2, -40, -7, -1, -1, -75, 3,
-1, -12, -36, 2, -4, -56, 3, -1, 1, -44, 3, -1, 0, -1, 2, 3
};

// BAND STOP FILTER
int8_t koef_bsf1[BUFFERLENGTH] = {
24, -10, -1, -4, 129, -4, -1, 10, 24, 2, 2, -18, -7, 1, -5, 97, -3,
1, 7, -33, -1, -1, -1, 10, 1, 3, 191, -2, 1, 14, -43, -3, -3
};

int8_t koef_bsf2[BUFFERLENGTH] = {
18, -10, -1, -3, 81, 1, -1, -6, -28, 2, 1, -1, 5, -1, -1, -75, -1,
-1, -3, 1, -4, 2, 36, -23, -1, 1, -44, -1, -1, 1, 40, 3, 2
};

int8_t koef_bsf3[BUFFERLENGTH] = {
12, 1, 1, 1, 1, 7, -1, -1, 31, 3, 1, 10, 18, -1, 3, 28, 6, -1, -1,
44, -3, 1, 1, 1, -2, -3, -1, 3, -1, -1, 24, -1, 1
};

int8_t koef_bsf4[BUFFERLENGTH] = {
7, 18, 2, 2, -26, -7, -1, 3, 225, 2, 1, 10, 10, -1, -1, 10, -10, 1,
2, 64, 2, -1, -14, 43, 3, 3, 33, -10, 1, 1, 193, -3, -1
};

int8_t koef_bsf5[BUFFERLENGTH] = {
3, 40, 1, 1, -1, -5, 1, -3, 31, -1, -1, 6, -28, 3, -1, -18, 10, -1,
-3, 44, -1, -1, -1, -40, -3, -2, 36, -23, -1, -5, 24, -1, -1
};

// Prototype Fungsi
int filter_int(int input, int* koef);

int main(int argc, char *argv[]) {

    int buffer_offset = 0;
    int buffer_int[BUFFERLENGTH];

    /*=============================================================*/
    /*                   Creating Sinusoid Input                   */
    /*=============================================================*/
    // Deklarasi variabel

    switch (atoi(argv[2])) {
        case 1:
            koef_filter_int = koef_lpf1;
            break;
        case 2:
            koef_filter_int = koef_lpf2;
            break;
        case 3:
            koef_filter_int = koef_lpf3;
            break;
        case 4:
            koef_filter_int = koef_lpf4;
            break;
        case 5:
            koef_filter_int = koef_lpf5;
            break;
        case 6:
            koef_filter_int = koef_lpf6;
            break;
        case 7:
            koef_filter_int = koef_lpf7;
            break;
        case 8:
            koef_filter_int = koef_hpf1;
            break;
        case 9:
            koef_filter_int = koef_hpf2;
            break;
        case 10:
            koef_filter_int = koef_hpf3;
            break;
        case 11:
            koef_filter_int = koef_hpf4;
            break;
        case 12:
            koef_filter_int = koef_hpf5;
            break;
        case 13:
            koef_filter_int = koef_hpf6;
            break;
        case 14:
            koef_filter_int = koef_hpf7;
            break;
        case 15:
            koef_filter_int = koef_bpf1;
            break;
        case 16:
            koef_filter_int = koef_bpf2;
            break;
        case 17:
            koef_filter_int = koef_bpf3;
            break;
        case 18:
            koef_filter_int = koef_bpf4;
            break;
        case 19:
            koef_filter_int = koef_bpf5;
            break;
        case 20:
            koef_filter_int = koef_bsf1;
            break;
        case 21:
            koef_filter_int = koef_bsf2;
            break;
        case 22:
            koef_filter_int = koef_bsf3;
            break;
        case 23:
            koef_filter_int = koef_bsf4;
            break;
        case 24:
            koef_filter_int = koef_bsf5;
            break;
        default:
            koef_filter_int = koef_lpf1;
            break;
    }


    float holder;
    int f1 = atoi(argv[1]); // Frekuensi sinyal masukan 1
    int f2 = 0; // Frekuensi sinyal masukan 2
    int f3 = 0; // Frekuensi sinyal masukan 3
    int fs = 8000; // Frekuensi sampling
    FILE *pFile_int;
    pFile_int = fopen("Sinyal Input.txt", "w");
    int i =0;
    for(i=0; i < LENGTH; i++) {
        holder = ((sin(2*M_PI*f1/fs*i))+(sin(2*M_PI*f2/fs*i))+(sin(2*M_PI*f3/fs*i))); // Menjumlahkan 3 sinus lalu amplitudanya dinormalisasi
		x_int[i] = (int) (holder*pow(2,12)); // Dikonversi ke bilangan fraksional dipilih 15 agar tidak terjadi overflow
        fprintf(pFile_int,"%f\n", holder);
	}
    fclose(pFile_int);

	/*=============================================================*/
    /*                             Setup                           */
	/*=============================================================*/
	// Inisialisasi buffer dengan nol
	for(i=0; i < BUFFERLENGTH; i++) {
		buffer_int[i] = 0;
	}

	/*=============================================================*/
    /*                      Filtering Process                      */
    /*=============================================================*/
    // Proses Filtering
	pFile_int = fopen("Sinyal Output.txt", "w");
    // Main Loop kita
	for(i=0; i < LENGTH; i++) {
		// y_int[i] = filter_int(x_int[i], koef_filter_int);

        int result = 0;

        buffer_int[buffer_offset] = x_int[i];

        int j;
        // Circular Convolution
        for(j = 0; j < BUFFERLENGTH; j++) {
            result += buffer_int[(j+buffer_offset)  % BUFFERLENGTH] * koef_filter_int[j];
        }

        // Geser Buffer Offset
        if (buffer_offset<=0){
            buffer_offset += BUFFERLENGTH-1;
        } else {
            buffer_offset--;
        };
        y_int[i] = result;

		fprintf(pFile_int,"%d\n", (uint8_t)( (y_int[i] >> 13) + 128 ));
	}
	fclose(pFile_int);

	return 0;
}


/*=============================================================*/
/*                     Function Definition                     */
/*=============================================================*/

    // int filter_int(int input, int *koef) {
    //     int result = 0;

    //     buffer_int[buffer_offset] = input;

    //     int j;
    //     // Circular Convolution
    //     for(j = 0; j < BUFFERLENGTH; j++) {
    //         result += buffer_int[(j+buffer_offset)  % BUFFERLENGTH] * koef[j];
    //     }

    //     // Geser Buffer Offset
    //     if (buffer_offset<=0){
    //         buffer_offset += BUFFERLENGTH-1;
    //     } else {
    //         buffer_offset--;
    //     };
    //     return result;
    // }

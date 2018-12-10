#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define BUFFERLENGTH 33
#define LENGTH 200

int buffer_offset = 0;
int buffer_int[BUFFERLENGTH];
int x_int[LENGTH];
int y_int[LENGTH];


/*=============================================================*/
/*                       Koefisien Filter                      */
/*=============================================================*/
/*   Koefisien Filter merupakan bilangan fraksional dengan     */
/*   pengali bernilai 2 pangkat 15.                            */
/*   Diperoleh dari MATLAB dengan menggunakan fungsi fir1().   */
/*=============================================================*/

int koef_filter_int[BUFFERLENGTH] = {
-31,0,50,120,177,158,0,-307,-669,-892,-731,0,1326,3049,4784	6074,6550,6074,4784,3049,1326,0,-731,-892,-669,-307,0,158,177,120,50,0,-31
};

// Prototype Fungsi
int filter_int(int input);

int main(int argc, char *argv[]) {
    /*=============================================================*/
    /*                   Creating Sinusoid Input                   */
    /*=============================================================*/
    // Deklarasi variabel
    float temp;
    int f1 = 1; // Frekuensi sinyal masukan 1
    int f2 = 8; // Frekuensi sinyal masukan 2
    int f3 = 50;
    int fs = 200; // Frekuensi sampling
    FILE *pFile_int;
    pFile_int = fopen("Input.txt", "w");
    for(int i=0; i < LENGTH; i++) {
        temp = ((sin(2*M_PI*f1/fs*i))+(sin(2*M_PI*f2/fs*i))+0.5*(sin(2*M_PI*f3/fs*i)))/(2.5); // Menjumlahkan 3 sinus lalu amplitudanya dinormalisasi
		x_int[i] = (int) (temp*pow(2,15)); // Dikonversi ke bilangan fraksional dipilih 15 agar tidak terjadi overflow
        fprintf(pFile_int,"%4.4f\n", temp);
	}
    fclose(pFile_int);

	/*=============================================================*/
    /*                             Setup                           */
	/*=============================================================*/
	// Inisialisasi buffer dengan nol
	for(int i=0; i < BUFFERLENGTH; i++) {
		buffer_int[i] = 0;
	}

	/*=============================================================*/
    /*                      Filtering Process                      */
    /*=============================================================*/
    // Proses Filtering
	pFile_int = fopen("Output.txt", "w");
    // Main Loop kita
	for(int i=0; i < LENGTH; i++) {
		y_int[i] = filter_int(x_int[i]);
		fprintf(pFile_int,"%4.4f\n", ((float) y_int[i])/pow(2,30));
	}
	fclose(pFile_int);

	return 0;
}


/*=============================================================*/
/*                     Function Definition                     */
/*=============================================================*/

int filter_int(int input) {
    int result = 0;

    buffer_int[buffer_offset] = input;

    // Circular Convolution
    for(int j = 0; j < BUFFERLENGTH; j++) {
        result += buffer_int[(j+buffer_offset)  % BUFFERLENGTH] * koef_filter_int[j];
    }

    // Geser Buffer Offset
    if (buffer_offset<=0){
        buffer_offset += BUFFERLENGTH-1;
    } else {
        buffer_offset--;
    };
    return result;
}

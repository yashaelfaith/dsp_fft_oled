#include "FSM.h"

void fsm1(int but_1, int but_2, int rot, int* change, State* state) {
    // State Handler
    switch (*state){
        case Display_Info:
            if (but_1 == 1){
                *state = Show_FFT;
            }
            break;
        case Show_FFT:
            if (but_1 == 1){
                *state = Display_Info;
            }
            else if (but_2 == 1){
                *state = Chg_Type;
            }
            else if (rot == 1){
                *state = Chg_Freq;
            }
            break;
        case Chg_Type:
            if (but_1 == 1){
                *state = Display_Info;
            } else {
                *state = Show_FFT;
            }
            break;
        case Chg_Freq:
            if (but_1 == 1){
                *state = Display_Info;
            } else {
                *state = Show_FFT;
            }
            break;
        default:
            break;
    }

    // Output Handler
    switch (*state){
        case Display_Info:
            *change = 0;
            break;
        case Show_FFT:
            *change = 0;
            break;
        case Chg_Type:
            *change = 1;
            break;
        case Chg_Freq:
            *change = 1;
            break;
        default:
            *change = 0;
            break;
    }
}


void fsm2(int input, State* state) {
    // State Handler
    switch (*state){
        case Compute_Filter:
            if (input == 1){
                *state = Chg_Coeff;
            }
            break;
        case Chg_Coeff:
            *state = Compute_Filter;
            break;
        default:
            break;
    }
}

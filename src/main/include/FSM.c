#include "FSM.h"

void fsm1(int8_t but_1, int8_t but_2, int8_t rot, int8_t* change, State* state) {
    // State Handler
    switch (*state){
        case Display_Info:
            if (but_1 == 1){
                // printf("d_ifo->sh_f\n");
                *state = Show_FFT;
            }
            break;
        case Show_FFT:
            if (but_1 == 1){
                // printf("sh_f->d_ifo\n");
                *state = Display_Info;
            }
            else if (but_2 == 1){
                // printf("sh_f->c_tp\n");
                *state = Chg_Type;
            }
            else if (rot == 1){
                // printf("sh_f->c_fr\n");
                *state = Chg_Freq;
            }
            break;
        case Chg_Type:
            if (but_1 == 1){
                // printf("c_tp->d_ifo\n");
                *state = Display_Info;
            } else {
                // printf("c_tp->sh_f\n");
                *state = Show_FFT;
            }
            break;
        case Chg_Freq:
            if (but_1 == 1){
                // printf("c_fr->d_ifo\n");
                *state = Display_Info;
            } else {
                // printf("c_fr->sh_f\n");
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


void fsm2(int8_t input, State* state) {
    // State Handler
    switch (*state){
        case Compute_Filter:
            if (input == 1){
                // printf("cm_f->c_coef\n");
                *state = Chg_Coeff;
            }
            break;
        case Chg_Coeff:
            // printf("c_coef->cm_f\n");
            *state = Compute_Filter;
            break;
        default:
            break;
    }
}

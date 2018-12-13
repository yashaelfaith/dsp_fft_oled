// #include <stdio.h>
#include <stdint.h>

#ifndef FSM_H
#define FSM_H

typedef enum State {
    Display_Info,
    Show_FFT,
    Chg_Type,
    Chg_Freq,
    Compute_Filter,
    Chg_Coeff,
} State;

void fsm1(int8_t but_1, int8_t but_2, int8_t rot, int8_t* change, State* state);
void fsm2(int8_t input, State* state);

#endif

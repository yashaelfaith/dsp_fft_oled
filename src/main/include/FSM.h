#ifndef FSM3_H
#define FSM3_H
#endif

typedef enum State {
    Display_Info,
    Show_FFT,
    Chg_Type,
    Chg_Freq,
    Compute_Filter,
    Chg_Coeff,
} State;

void fsm1(int but_1, int but_2, int rot, State* state);
void fsm2(int input, State* state);

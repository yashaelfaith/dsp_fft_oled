// Include Libraries
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "driver/dac.h"
#include "esp_log.h"

#include "include/FSM.h"
#include "include/fix_fft.h"
#include "include/filter_coeff.h"


// Define
#define ADC_CHANNEL ADC1_CHANNEL_0
#define DAC_CHANNEL DAC_CHANNEL_1
#define BUTTON1 GPIO_NUM_18
#define BUTTON2 GPIO_NUM_5
#define ROT_BUTTON GPIO_NUM_16
#define ROT_A GPIO_NUM_0
#define ROT_B GPIO_NUM_2
#define BUZZER GPIO_NUM_4

#define FSM_PERIOD 3   // 2 ms
#define FILTER_PERIOD 1 // 1 ms -> 1kHz
#define FFT_PERIOD 128 // 128*FILTER_PERIOD (use more to check FFT output via serial)


// Global Variables
State display_state = Display_Info;
State filter_state = Compute_Filter;
QueueHandle_t xQueue;
const int8_t* used_coeff;
int8_t rot_count = 0;


// Function Prototypes
// void main_task(void *pvParameter);
void filter_task(void *pvParameter);
void fsm_task(void *pvParameter);
void fft_task(void *pvParameter);
void initialize_adc(void);
void initialize_dac(void);
void initialize_gpio(void);


int app_main() {

    // struct timeval t1, t2;
    // double elapsedTime;
    // gettimeofday(&t1, NULL);

    initialize_adc();
    initialize_dac();
    initialize_gpio();

    xQueue = xQueueCreate( 128, sizeof(int8_t) ); // Use more to check FFT output via serial

    // gettimeofday(&t2, NULL);
    // elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000000.0;  // sec to us
    // elapsedTime += (t2.tv_usec - t1.tv_usec) ;
    // ESP_LOGW("INIT", "%lf us", elapsedTime);

    // xTaskCreate(&main_task, "main_task", configMINIMAL_STACK_SIZE, NULL, 5, NULL);
    xTaskCreate(&filter_task, "filter_task", 2048, NULL, 6, NULL);
    xTaskCreate(&fsm_task,  "fsm_task", 2048, NULL, 7, NULL);
    xTaskCreate(&fft_task, "fft_task", 2048, NULL, 5, NULL);

    return 0;
}


// void main_task(void *pvParameter) {

// }


void filter_task(void *pvParameter) {

    TickType_t xLastWakeTime;
    const TickType_t Period = FILTER_PERIOD / portTICK_PERIOD_MS;
    
    int adc_reading;
    int buffer_offset = 0;
    int buffer_int[BUFFERLENGTH];
    int j;

    // Initialise the xLastWakeTime variable with the current time.
    xLastWakeTime = xTaskGetTickCount();

    // Initializing Buffer
    for(j = 0; j < BUFFERLENGTH; j++) {
        buffer_int[j] = 0;
    }

    // Initializing Used Filter Coefficient
    used_coeff = lpf1;

    // struct timeval t1, t2;
    // double elapsedTime;

    while (1) {
        // gettimeofday(&t1, NULL);

        if (filter_state == Chg_Coeff) {
            // printf("%d\n", rot_count);  // Uncomment to see current state

            // Change used_coeff
            switch(rot_count) {
                case 0:
                    used_coeff = apf0;
                    break;
                case 1:
                    used_coeff = lpf1;
                    break;
                case 2:
                    used_coeff = lpf2;
                    break;
                case 3:
                    used_coeff = lpf3;
                    break;
                case 4:
                    used_coeff = lpf4;
                    break;
                case 5:
                    used_coeff = lpf5;
                    break;
                case 6:
                    used_coeff = lpf6;
                    break;
                case 7:
                    used_coeff = lpf7;
                    break;
                case 8:
                    used_coeff = hpf1;
                    break;
                case 9:
                    used_coeff = hpf2;
                    break;
                case 10:
                    used_coeff = hpf3;
                    break;
                case 11:
                    used_coeff = hpf4;
                    break;
                case 12:
                    used_coeff = hpf5;
                    break;
                case 13:
                    used_coeff = hpf6;
                    break;
                case 14:
                    used_coeff = hpf7;
                    break;
                case 15:
                    used_coeff = bpf1;
                    break;
                case 16:
                    used_coeff = bpf2;
                    break;
                case 17:
                    used_coeff = bpf3;
                    break;
                case 18:
                    used_coeff = bpf4;
                    break;
                case 19:
                    used_coeff = bpf5;
                    break;
                case 20:
                    used_coeff = bsf1;
                    break;
                case 21:
                    used_coeff = bsf2;
                    break;
                case 22:
                    used_coeff = bsf3;
                    break;
                case 23:
                    used_coeff = bsf4;
                    break;
                case 24:
                    used_coeff = bsf5;
                    break;
                default:
                    used_coeff = lpf1;
                    break;
            }
        } else {
            // Compute Filter
            // Read ADC, 12 Bit
            adc_reading = adc1_get_raw(ADC_CHANNEL);

            // Conditioning Variables
            int result = 0;
            buffer_int[buffer_offset] = adc_reading;

            // Circular Convolution Process, result max 20 Bit
            for(j = 0; j < BUFFERLENGTH; j++) {
                result += buffer_int[(j+buffer_offset)  % BUFFERLENGTH] * used_coeff[j];
            }

            // Geser Buffer Offset
            if (buffer_offset<=0){
                buffer_offset += BUFFERLENGTH-1;
            } else {
                buffer_offset--;
            };

            // Convert Result for FFT to 8 Bit
            // by shift 10 times and ad 2^7
            if (rot_count == 0) {
                result = (int8_t) (adc_reading >> 5);
            } else {
                result = (int8_t) (result >> 13);
            }

            // Enqueue Result
            xQueueSendToBack(xQueue, (void *) &result, (TickType_t) 0);

            // Convert Result for DAC to 8 Bit,
            // by shift 13 OR 5 times and ad 2^7
            if (rot_count == 0) {
                result = (uint8_t) ((adc_reading >> 5) + 128);
            } else {
                result = (uint8_t) ((result >> 13) + 128);
            }

            // Write DAC
            dac_output_voltage(DAC_CHANNEL, result);

            // Calculate exec time
            // gettimeofday(&t2, NULL);
            // elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000000.0;  // sec to us
            // elapsedTime += (t2.tv_usec - t1.tv_usec) ;
            // ESP_LOGW("FILTER", "%lf us, r:%d", elapsedTime, rot_count);

        }
        // Task Period
        vTaskDelayUntil(&xLastWakeTime, Period);
    }
}


void fsm_task(void *pvParameter) {

    // Task's Period variables
    TickType_t xLastWakeTime;
    const TickType_t Period = FSM_PERIOD / portTICK_PERIOD_MS;

    // Button previous & current reading
    int8_t prev_A = 1, prev_but_1 = 0, prev_but_2 = 0, prev_rot_but = 1;
    int8_t curr_A, curr_B, curr_but_1, curr_but_2, curr_rot_but;

    // FSM inputs & outputs signals
    int8_t but_1, but_2, rot = 0;
    int8_t rot_but, change, in_fsm2;

    // rot_count additional variables
    const int8_t type[5] = {0, 1, 8, 15,20};
    const int8_t max_of_type [5] = {0, 7, 14, 19, 24};
    int8_t current = 0;

    // struct timeval t1, t2;
    // double elapsedTime;

    // Initialise the xLastWakeTime variable with the current time.
    xLastWakeTime = xTaskGetTickCount();

    while(1) {

        // gettimeofday(&t1, NULL);

        // Reading Buttons current state
        curr_but_1 = gpio_get_level(BUTTON1);
        curr_but_2 = gpio_get_level(BUTTON2);
        curr_rot_but = gpio_get_level(ROT_BUTTON);

        // Creating fsm input signal
        but_1 = ((prev_but_1 == 0) && (curr_but_1 == 1));
        but_2 = ((prev_but_2 == 0) && (curr_but_2 == 1));
        rot_but = ((prev_rot_but == 1) && (curr_rot_but == 0));

        // Reading Rotary Encoder
        curr_B = gpio_get_level(ROT_B);
        curr_A = gpio_get_level(ROT_A);
        if ((prev_A == 0) && (curr_A == 1)) {
            rot = 1;
            if (curr_B == 0) {
                rot_count = rot_count + 1;
            } else {
                rot_count = rot_count - 1;
            }
            // Clipping rot_count between 1 to 7
            if (rot_count < type[current]) {
                rot_count = type[current];
            } else if ( rot_count > max_of_type[current]) {
                rot_count = max_of_type[current];
            }
            // printf("%d\n", rot_count);
        } else {
            rot = 0;
        }

        // Turn on Buzzer on each signal
        gpio_set_level(BUZZER, but_1|but_2|rot_but|rot);

        // Running FSM
        fsm1(but_1, but_2, rot, &change, &display_state);
        in_fsm2 = change | rot_but;
        fsm2(in_fsm2, &filter_state);

        // Changes Filter type
        if (rot_but) {
            current = 0;
            rot_count = type[current];
        } else if (change && but_2) {
            current = (current + 6) % 5;
            rot_count = type[current];
        }

        // Update button previous state
        prev_but_1 = curr_but_1;
        prev_but_2 = curr_but_2;
        prev_rot_but = curr_rot_but;
        prev_A = curr_A;

        // Calculate exec time
        // gettimeofday(&t2, NULL);
        // elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000000.0;  // sec to us
        // elapsedTime += (t2.tv_usec - t1.tv_usec) ;
        // ESP_LOGW("FSM", "%lf us", elapsedTime);

        // Task Period
        vTaskDelayUntil(&xLastWakeTime, Period);
    }
}


void fft_task(void *pvParameter) {

    TickType_t xLastWakeTime;
    const TickType_t Period = FFT_PERIOD / portTICK_PERIOD_MS;

    int8_t filt_res[128];
    int8_t im[128];
    int i = 0;
    int8_t fft_res[128];

    // struct timeval t1, t2;
    // double elapsedTime;

    // Initialise the xLastWakeTime variable with the current time.
     xLastWakeTime = xTaskGetTickCount();

    while (1) {

        // gettimeofday(&t1, NULL);

        // Resetting Im & Inserting Re
        // printf("is\n"); // Input signal, start sign
        for (i = 0; i < 128; i++)
        {
            im[i] = 0;
            if(xQueueReceive(xQueue, &filt_res[i], (TickType_t) 0)) {
                // printf("%d\n", filt_res[i]);      // Uncomment to check input signal
            }
        }
        // printf("if\n"); // Input signal, finish sign

        // Calculate FFT
        fix_fft(filt_res, im, 7, 0);
        // printf("fs\n"); // FFT signal, start sign
        for (i = 0; i < 64; i++) {
            fft_res[i] = 2*isqrt(filt_res[i] * filt_res[i] + im[i] * im[i]);
            // printf("%d\n", fft_res[i]);         // Uncomment to check FFT signal
        }
        // printf("ff\n"); // FFT signal, finish sign

        // gettimeofday(&t2, NULL);
        // elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000000.0;  // sec to us
        // elapsedTime += (t2.tv_usec - t1.tv_usec) ;
        // ESP_LOGW("FFT", "%lf us", elapsedTime);

        // Task Period
        vTaskDelayUntil(&xLastWakeTime, Period);
    }
}


void initialize_adc(void) {
    // ADC Configuration
    // 12 Bit Resolution
    // 6 DB Attenuation gives 2.2 V Full Swing
    adc1_config_width(ADC_WIDTH_BIT_12 );
    adc1_config_channel_atten(ADC_CHANNEL, ADC_ATTEN_DB_6);
}


void initialize_dac(void) {
    // DAC Configuration
    dac_output_enable(DAC_CHANNEL);
}


void initialize_gpio(void) {
    // GPIO Input Configuration
    gpio_config_t io_conf;
    // disable interrupt
    io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
    // set as output mode
    io_conf.mode = GPIO_MODE_INPUT;
    // Button pin's bit mask
    io_conf.pin_bit_mask = (1ULL << BUTTON1) | (1ULL << BUTTON2);
    // enable pull-down mode & disable pull-up mode
    io_conf.pull_down_en = 1;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);

    // Rotary Encoder pin's bit mask
    io_conf.pin_bit_mask = (1ULL << ROT_BUTTON) | (1ULL << ROT_A) | (1ULL << ROT_B);
    // disable pull-down mode & // enable pull-up mode
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 1;
    gpio_config(&io_conf);

    // GPIO Output Configuration
    io_conf.mode = GPIO_MODE_OUTPUT;
    // Output pin bit mask
    io_conf.pin_bit_mask = (1 << BUZZER);
    // disable pull-up mode
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);
}


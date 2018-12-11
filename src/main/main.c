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
#define ADC_CHANNEL ADC1_CHANNEL_6
#define DAC_CHANNEL DAC_CHANNEL_1
#define BUTTON1 GPIO_NUM_5
#define BUTTON2 GPIO_NUM_17
#define ROT_BUTTON GPIO_NUM_4
#define ROT_A GPIO_NUM_2
#define ROT_B GPIO_NUM_15
#define BUZZER GPIO_NUM_0

#define FSM_PERIOD 1000
#define FILTER_PERIOD 1000

// Global Variables
State display_state;
State filter_state;
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

    struct timeval t1, t2;
    double elapsedTime;
    gettimeofday(&t1, NULL);

    initialize_adc();
    initialize_dac();
    initialize_gpio();

    xQueue = xQueueCreate( 128, sizeof(int) );

    gettimeofday(&t2, NULL);
    elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000000.0;      // sec to us
    elapsedTime += (t2.tv_usec - t1.tv_usec) ;   // us to ms
    ESP_LOGW("INIT", "%lf us", elapsedTime);

    // xTaskCreate(&main_task, "main_task", configMINIMAL_STACK_SIZE, NULL, 5, NULL);
    // xTaskCreate(&filter_task, "filter_task", 2048, NULL, 6, NULL);
    // xTaskCreate(&fsm_task,  "fsm_task", 2048, NULL, 7, NULL);
    // xTaskCreate(&fft_task, "fft_task", 2048, NULL, 5, NULL);

    return 0;
}


// void main_task(void *pvParameter) {

// }


void filter_task(void *pvParameter) {

    int adc_reading;
    int buffer_offset = 0;
    int buffer_int[BUFFERLENGTH];
    int j;

    // Initializing Buffer
    for(j = 0; j < BUFFERLENGTH; j++) {
        buffer_int[j] = 0;
    }

    // Initializing Used Filter Coefficient
    used_coeff = lpf1;

    while (1) {

        if (filter_state == Chg_Coeff) {
            // Change used_coeff
            switch(rot_count) {
                case 0:
                    used_coeff = lpf1;
                    break;
                case 1:
                    used_coeff = lpf2;
                    break;
                case 2:
                    used_coeff = lpf3;
                    break;
                case 3:
                    used_coeff = lpf4;
                    break;
                case 4:
                    used_coeff = lpf5;
                    break;
                case 5:
                    used_coeff = lpf6;
                    break;
                case 6:
                    used_coeff = lpf7;
                    break;
                case 7:
                    used_coeff = hpf1;
                    break;
                case 8:
                    used_coeff = hpf2;
                    break;
                case 9:
                    used_coeff = hpf3;
                    break;
                case 10:
                    used_coeff = hpf4;
                    break;
                case 11:
                    used_coeff = hpf5;
                    break;
                case 12:
                    used_coeff = hpf6;
                    break;
                case 13:
                    used_coeff = hpf7;
                    break;
                case 14:
                    used_coeff = bpf1;
                    break;
                case 15:
                    used_coeff = bpf2;
                    break;
                case 16:
                    used_coeff = bpf3;
                    break;
                case 17:
                    used_coeff = bpf4;
                    break;
                case 18:
                    used_coeff = bpf5;
                    break;
                case 19:
                    used_coeff = bsf1;
                    break;
                case 20:
                    used_coeff = bsf2;
                    break;
                case 21:
                    used_coeff = bsf3;
                    break;
                case 22:
                    used_coeff = bsf4;
                    break;
                case 23:
                    used_coeff = bsf5;
                    break;
                default:
                    used_coeff = lpf1;
                    break;
            }
        } else {
            // Compute Filter
            // Read ADC, 9 Bit
            adc_reading = adc1_get_raw(ADC_CHANNEL);

            // Conditioning Variables
            int result = 0;
            buffer_int[buffer_offset] = adc_reading;

            // Circular Convolution Process, result max 17 Bit
            for(j = 0; j < BUFFERLENGTH; j++) {
                result += buffer_int[(j+buffer_offset)  % BUFFERLENGTH] * used_coeff[j];
            }

            // Geser Buffer Offset
            if (buffer_offset<=0){
                buffer_offset += BUFFERLENGTH-1;
            } else {
                buffer_offset--;
            };

            // Convert to 8 Bit, by shift 10 times and ad 2^7
            result = (uint8_t)( (result >> 10) + 128 );

            // Enqueue Result
            xQueueSendToBack(xQueue, (void *) &result, (TickType_t) 0);

            // Write DAC
            dac_output_voltage(DAC_CHANNEL, result);

            // Task Period
            vTaskDelay( FILTER_PERIOD / portTICK_PERIOD_MS);
        }
    }
}


void fsm_task(void *pvParameter) {

    int prev_A = 0;
    int curr_A = 0;
    int curr_B = 0;

    // FSM inputs & outputs
    int but_1, but_2, rot;
    int rot_but, change, in_fsm2;

    while(1) {
        // Reading Button
        but_1 = gpio_get_level(BUTTON1);
        but_2 = gpio_get_level(BUTTON2);
        rot_but = gpio_get_level(ROT_BUTTON);

        // Reading Rotary Encoder
        curr_A = gpio_get_level(ROT_A);
        curr_B = gpio_get_level(ROT_B);
        if (prev_A == 0 && curr_A == 1) {
            rot = 1;
            if (curr_B == 0) {
                rot_count = (rot_count + 1) % 24;
            } else {
                rot_count = (rot_count - 1) % 24;
            }
        } else {
            rot = 0;
        }

        fsm1(but_1, but_2, rot, &change, &display_state);
        in_fsm2 = change | rot_but;
        fsm2(in_fsm2, &filter_state);

        // Task Period
        vTaskDelay( FSM_PERIOD / portTICK_PERIOD_MS);
    }

}


// void fft_task(void *pvParameter) {

//     int8_t filt_res[128];
//     int8_t im[128];
//     int8_t i = 0;
//     int8_t fft_res[128];

//     struct timeval t1, t2;
//     double elapsedTime;
//     gettimeofday(&t1, NULL);

//     // while (1) {
//         // Resetting Im & Inserting Re
//         for (i = 0; i < 128; ++i)
//         {
//             im[i] = 0;
//             xQueueReceive(xQueue, (void *) filt_res[i], (TickType_t) 0);
//         }

//         // Calculate FFT
//         fix_fft(filt_res, im, 7, 0);
//         for (i = 0; i < 64; i++) {
//             fft_res[i] = 2*isqrt(filt_res[i] * filt_res[i] + im[i] * im[i]);
//         }

//         // Task Period
//         vTaskDelay( 10 / portTICK_PERIOD_MS);
//     // }

//     gettimeofday(&t2, NULL);
//     elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000000.0;      // sec to us
//     elapsedTime += (t2.tv_usec - t1.tv_usec) ;   // us to ms
//     ESP_LOGW("FFT", "%lf us", elapsedTime);
// }


void initialize_adc(void) {
    // ADC Configuration
    // 9 Bit Resolution
    // 6 DB Attenuation gives 2.2 V Full Swing
    adc1_config_width(ADC_WIDTH_BIT_9 );
    adc1_config_channel_atten(ADC_CHANNEL, ADC_ATTEN_DB_6);
}


void initialize_dac(void) {
    // DAC Configuration
    dac_output_enable(DAC_CHANNEL);
}


void initialize_gpio(void) {
    // GPIO Input Configuration
    gpio_config_t io_conf;
    //disable interrupt
    io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
    //set as output mode
    io_conf.mode = GPIO_MODE_INPUT;
    //bit mask of the pins that you want to set,e.g.GPIO18/19
    io_conf.pin_bit_mask = (1ULL << BUTTON1) | (1ULL << BUTTON2) | (1ULL << ROT_BUTTON) | (1ULL << ROT_A) | (1ULL << ROT_B);
    //disable pull-down mode
    io_conf.pull_down_en = 0;
    //enable pull-up mode
    io_conf.pull_up_en = 1;
    //configure GPIO with the given settings
    gpio_config(&io_conf);

    // GPIO Output Configuration
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1 << BUZZER);
    //enable pull-up mode
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);
}


// Include Libraries
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#include "include/FSM.h"
#include "include/fix_fft.h"


// Define
#define ADC_CHANNEL ADC1_CHANNEL_6
#define DAC_CHANNEL DAC_CHANNEL_1
#define BUTTON1 GPIO_NUM_0
#define BUTTON2 GPIO_NUM_0
#define ROT_BUTTON GPIO_NUM_0
#define ROT_A GPIO_NUM_0
#define ROT_B GPIO_NUM_0
#define BUZZER GPIO_NUM_0




// Global Variables


// Function Prototypes
void main_task(void *pvParameter);
void filter_task(void *pvParameter);
void fsm_task(void *pvParameter);
void fft_task(void *pvParameter);
void initialize_adc(void);
void initialize_dac(void)
void initialize_gpio(void);



int app_main()
{
    initialize_adc();
    initialize_dac();
    initialize_gpio();

    xTaskCreate(&main_task, "main_task", configMINIMAL_STACK_SIZE, NULL, 5, NULL);
    xTaskCreate(&filter_task, "filter_task", configMINIMAL_STACK_SIZE, NULL, 5, NULL);
    xTaskCreate(&fsm_task, "fsm_task", configMINIMAL_STACK_SIZE, NULL, 5, NULL);
    xTaskCreate(&fft_task, "fft_task", configMINIMAL_STACK_SIZE, NULL, 5, NULL);
}


void main_task(void *pvParameter) {

}


void filter_task(void *pvParameter) {

    // QueueHandle_t xQueue;
    // int adc_reading;

    // while (1) {
    //     adc_reading = adc1_get_raw(ADC_CHANNEL);


    // }

}


void fsm_task(void *pvParameter) {

    // Might be Global Variables
    int rot_count = 0;
    int rot_but;
    State* display_state;
    State* filter_state;

    int prev_A = 0;
    int curr_A = 0;
    int curr_B = 0;

    // FSM inputs & outputs
    int but_1, but_2, rot, change;
    int in_fsm2;

    while(1) {
        // Reading Button
        but_1 = gpio_get_level(BUTTON1);
        but_2 = gpio_get_level(BUTTON2);
        rot_but = gpio_get_level(ROT_BUTTON);

        // Reading Rotary Encoder
        curr_A = gpio_get_level(ROT_A);
        curr_B = gpio_get_level(ROT_B);
        if (prev_A == 0 && curr_A = 1) {
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
        vTaskDelay( 10 / portTICK_PERIOD_MS);
    }
}


void fft_task(void *pvParameter) {

}


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
    gpio_config(&io_conf);
}


#include "stm32f4xx_hal.h"



void adc_pa0_continuous_conversion_init(void) {
    // we will use PA0 as the ADC input pin, which corresponds to ADC1_1 (first channel of ADC1)

    // configure PA0 as an analog input pin
    __HAL_RCC_GPIOA_CLK_ENABLE(); // enable clock access to GPIOA



    // configure ADC module for continuous conversion
    __HAL_RCC_ADC1_CLK_ENABLE(); // enable clock access to ADC1
}
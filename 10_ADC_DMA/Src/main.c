#include "stm32f4xx_hal.h"
#include <stdint.h>
#include <stdio.h>

#define BTN_PORT GPIOC
#define BTN_PIN GPIO_PIN_13
#define LED_PORT GPIOD
#define LED_PIN GPIO_PIN_12

void gpio_pc13_interrupt_init(void);

int main() {
  HAL_Init();
  uart_init();
  adc_pa0_interrupt_init(void);

  // start ADC
  // we have already configured the ADC to run in continuous conversion mode, so we just need to start it once and it will keep converting and generating interrupts whenever a conversion is complete.
  HAL_ADC_Start_IT(&hadc1); // start ADC in interrupt mode, so that it will generate an interrupt whenever a conversion is complete and the data is ready to be read.


  while (1) {

  }
  
}

// automatically called by the HAL library when the ADC finishes a conversion (triggered by the interrupt handler)
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
    if (hadc->Instance == ADC1) { // check if the interrupt is from ADC1
        uint32_t adc_value = p0_adc_read(); // read the converted value from the ADC data register using the p0_adc_read() function, which returns the value as a 32-bit unsigned integer.
        printf("ADC Value: %lu\n", adc_value); // print the ADC value to the console using printf. %lu is used to format the unsigned long integer returned by p0_adc_read().
    }

}



void SysTick_Handler(void) {
  // whenever there is a SysTick interrupt, this function will be called. 
  // This function is defined weakly in the HAL library, so we can override it here to provide our own implementation. 
  // In this case, we will just call the HAL_IncTick() function to increment the tick count, which is used for timing functions in the HAL library.
  // a SysTick is a timer that generates an interrupt at regular intervals, which can be used for timing and scheduling tasks in an embedded system.
  HAL_IncTick();
}




#include "stm32f4xx_hal.h"
#include <stdint.h>
#include <stdio.h>
#include "uart.h"
#include "adc.h"


ADC_HandleTypeDef hadc1;
uint32_t sensor_value;

int main() {dddd
  HAL_Init();
  uart_init();

  adc_pa0_single_conversion_init();

  while (1) {

    // the difference with single conversion is that we need to start the ADC conversion process and then poll for the conversion complete flag before reading the converted value. 
    // in continuous conversion mode, the ADC will automatically start a new conversion after each one is complete, so we can just read the converted value whenever we want without having to start the conversion process again.

    // start ADC 
    HAL_ADC_Start(&hadc1); // this function starts the ADC conversion process. it sets the appropriate bits in the ADC control register to begin the conversion of the selected channel.

    // poll for conversion complete
    HAL_ADC_PollForConversion(&hadc1, 1); // this function polls for the end of the ADC conversion. it waits until the conversion is complete and the EOC (End of Conversion) flag is set in the ADC status register. the second parameter specifies the timeout duration, and in this case, we use HAL_MAX_DELAY to wait indefinitely until the conversion is complete.

    // get converted value
    sensor_value = p0_adc_read();


    printf("Sensor Value: %lu\n", (unsigned long)sensor_value);
    HAL_Delay(1000); // delay is the time between each transmission of the message
  }
  
}




void SysTick_Handler(void) {
  // whenever there is a SysTick interrupt, this function will be called. 
  // This function is defined weakly in the HAL library, so we can override it here to provide our own implementation. 
  // In this case, we will just call the HAL_IncTick() function to increment the tick count, which is used for timing functions in the HAL library.
  // a SysTick is a timer that generates an interrupt at regular intervals, which can be used for timing and scheduling tasks in an embedded system.
  HAL_IncTick();
}


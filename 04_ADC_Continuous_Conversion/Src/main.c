#include "stm32f4xx_hal.h"
#include <stdint.h>
#include <stdio.h>
#include "uart.h"






int main() {dddd
  HAL_Init();
  uart_init();

  while (1) {
    printf("%s", message);
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


#include "stm32f4xx_hal.h"
#include <stdint.h>
#include <stdio.h>


USART_HandleTypeDef huart2;
void uart_init(void);

char message[20] = "Hello, World!\r\n";

int __io_putchar(int ch) {
  // this function is used by the printf function to output characters. 
  // it is defined as a weak function in the HAL library, so we can override it here to provide our own implementation. 
  // in this case, we will just call the HAL_UART_Transmit() function to transmit the character over UART.
  HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
  return ch;
}

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

void uart_init(void) {

  /*
   * note: the InitTypeDef is a member inside the HandleTypeDef
  */
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  // enable UART pins clock access
  __HAL_RCC_GPIOA_CLK_ENABLE();

  // enable UART clock access
  __HAL_RCC_USART2_CLK_ENABLE();

  // configure pins to act as alternate function pins for UART
  GPIO_InitStruct.Pin = GPIO_PIN_2 | GPIO_PIN_3; // PA2 = USART2_TX, PA3 = USART2_RX. the | operator is used to combine the two pin numbers into a single value that can be passed to the HAL_GPIO_Init() function.
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP; // alternate function push-pull mode. This means that the pin will be controlled by the alternate function (in this case, the USART peripheral) and will be in push-pull mode, which allows it to drive both high and low output levels.
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF7_USART2; // alternate function 7 is the USART2 peripheral. This tells the microcontroller to route the USART2 signals to these pins.
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  // configure UART peripheral
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  HAL_UART_Init(&huart2);

}
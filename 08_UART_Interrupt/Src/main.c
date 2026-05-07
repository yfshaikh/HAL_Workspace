#include "stm32f4xx_hal.h"
#include <stdint.h>


USART_HandleTypeDef huart1;
void uart_init(void);

char message[20] = "Hello, World!\r\n";

int main() {
  HAL_Init();
  uart_init();

  while (1) {
    // delay is the time between each transmission of the message
    HAL_UART_Transmit(&huart1, (uint8_t *)message, strlen(message), HAL_MAX_DELAY);
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
  __HAL_RCC_USART1_CLK_ENABLE();

  // configure pins to act as alternate function pins for UART
  GPIO_InitStruct.Pin = GPIO_PIN_9 | GPIO_PIN_10; // PA9 = USART1_TX, PA10 = USART1_RX. the | operator is used to combine the two pin numbers into a single value that can be passed to the HAL_GPIO_Init() function.
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP; // alternate function push-pull mode. This means that the pin will be controlled by the alternate function (in this case, the USART peripheral) and will be in push-pull mode, which allows it to drive both high and low output levels.
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF7_USART1; // alternate function 7 is the USART1 peripheral. This tells the microcontroller to route the USART1 signals to these pins.
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  // configure UART peripheral
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  HAL_UART_Init(&huart1);

}
#include "stm32f4xx_hal.h"
#include <stdint.h>


USART_HandleTypeDef huart1;
void uart_init(void);

uint8_t tx_buffer[10] = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100};
uint8_t rx_buffer[10];

uint32_t rx_counter, tx_counter;
/*
 * Called automatically when UART transmission completes (all bytes sent)
 * This is a weak callback function that you override to handle TX completion
 * In this case, we just increment a counter to track how many transmissions finished
 */void HAL_UART_TxCpltCallback(USART_HandleTypeDef *huart) {
  tx_counter++;
}
/*
 * Called automatically when UART reception completes (all expected bytes received)
 * This is a weak callback function that you override to handle RX completion
 * In this case, we just increment a counter to track how many receptions finished
 */void HAL_UART_RxCpltCallback(USART_HandleTypeDef *huart) {
  rx_counter++;
}

/*
 * Main program entry point
 * Initializes UART peripheral and sets up interrupt-driven transmission and reception
 * The while loop does nothing - all work is handled by UART interrupt callbacks
 */
int main() {
  HAL_Init();
  uart_init();
  
  HAL_UART_Transmit_IT(&huart1, (uint8_t *)tx_buffer, sizeof(tx_buffer), HAL_MAX_DELAY);
  HAL_UART_Receive_IT(&huart1, (uint8_t *)rx_buffer, sizeof(rx_buffer));

  while (1) {
    
  }
  
}




void SysTick_Handler(void) {
  // whenever there is a SysTick interrupt, this function will be called. 
  // This function is defined weakly in the HAL library, so we can override it here to provide our own implementation. 
  // In this case, we will just call the HAL_IncTick() function to increment the tick count, which is used for timing functions in the HAL library.
  // a SysTick is a timer that generates an interrupt at regular intervals, which can be used for timing and scheduling tasks in an embedded system.
  HAL_IncTick();
}

/*
 * Initializes UART1 peripheral with pins PA9 (TX) and PA10 (RX)
 * Configures baud rate, data format, and enables UART interrupts
 * Must be called before using HAL_UART_Transmit_IT() or HAL_UART_Receive_IT()
 */
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
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  HAL_UART_Init(&huart1);

  HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(USART1_IRQn);

}


/*
 * Interrupt handler automatically called by CPU when USART1 interrupt occurs
 * (triggered by TX/RX events: byte sent, byte received, errors, etc)
 * Delegates to HAL library to clear flags and determine what happened
 * HAL then calls appropriate callbacks: HAL_UART_TxCpltCallback() or HAL_UART_RxCpltCallback()
 */
void USART1_IRQHandler(void) {
  HAL_UART_IRQHandler(&huart1); // calls the STM32 HAL library callback function to handle the interrupt for USART1, which manages tasks like clearing interrupt flags and invoking any registered callbacks
}
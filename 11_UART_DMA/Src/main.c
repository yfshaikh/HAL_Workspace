#include "stm32f4xx_hal.h"
#include <stdint.h>


USART_HandleTypeDef huart1;
USART_HandleTypeDef huart2;
void uart1_init(void);
void uart2_init(void);

uint8_t tx_buffer1[10] = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100};
uint8_t rx_buffer1[10];

uint8_t tx_buffer2[10] = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100};
uint8_t rx_buffer2[10];

uint32_t rx_counter1, tx_counter1;
uint32_t rx_counter2, tx_counter2;
/*
 * Called automatically when UART transmission completes (all bytes sent)
 * This is a weak callback function that you override to handle TX completion
 * In this case, we just increment a counter to track how many transmissions finished
 */
 void HAL_UART_TxCpltCallback(USART_HandleTypeDef *huart) {
  if (huart->Instance == USART1) {
    tx_counter1++;
  } else if (huart->Instance == USART2) {
    tx_counter2++;
  }
}
/*
 * Called automatically when UART reception completes (all expected bytes received)
 * This is a weak callback function that you override to handle RX completion
 * In this case, we just increment a counter to track how many receptions finished
 */
 void HAL_UART_RxCpltCallback(USART_HandleTypeDef *huart) {
  if (huart->Instance == USART1) {
    rx_counter1++;
  } else if (huart->Instance == USART2) {
    rx_counter2++;
  }
}

/*
 * Main program entry point
 * Initializes UART peripheral and sets up interrupt-driven transmission and reception
 * The while loop does nothing - all work is handled by UART interrupt callbacks
 */
int main() {
  HAL_Init();
  uart1_init();
  uart2_init();

  HAL_UART_Transmit_IT(&huart1, (uint8_t *)tx_buffer1, sizeof(tx_buffer1), HAL_MAX_DELAY);
  HAL_UART_Receive_IT(&huart1, (uint8_t *)rx_buffer1, sizeof(rx_buffer1));

  HAL_UART_Transmit_IT(&huart2, (uint8_t *)tx_buffer2, sizeof(tx_buffer2), HAL_MAX_DELAY);

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
void uart1_init(void) {

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



void uart2_init(void) {

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
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  HAL_UART_Init(&huart2);

  HAL_NVIC_SetPriority(USART2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(USART2_IRQn);

}


/*
 * ============================================================================
 * NAMING CLARIFICATION: USART vs UART
 * ============================================================================
 * 
 * You may notice inconsistent naming in the code:
 * - Interrupt handlers: USART1_IRQHandler, USART2_IRQHandler (fixed hardware names)
 * - HAL functions: HAL_UART_Transmit_IT, HAL_UART_RxCpltCallback (fixed library names)
 * 
 * Why the mismatch?
 * - USART = Universal Synchronous/Asynchronous Receiver/Transmitter (hardware name)
 * - UART = Universal Asynchronous Receiver/Transmitter (library naming choice)
 * 
 * The STM32 microcontroller has USART peripherals, so the interrupt vector names
 * use USART. However, STMicroelectronics consolidated UART and USART support into
 * a single HAL driver called "UART" for simplicity. Most applications only use
 * asynchronous mode (basic serial communication), so naming everything HAL_UART_*
 * keeps the API simple and consistent across all STM32 platforms.
 * 
 * This is standard across all STM32 HAL code - don't change the names, they are
 * defined by the hardware and library respectively.
 * ============================================================================
 */

/*
 * Interrupt handler automatically called by CPU when USART1 interrupt occurs
 * (triggered by TX/RX events: byte sent, byte received, errors, etc)
 * Delegates to HAL library to clear flags and determine what happened
 * HAL then calls appropriate callbacks: HAL_UART_TxCpltCallback() or HAL_UART_RxCpltCallback()
 */
void USART1_IRQHandler(void) {
  HAL_UART_IRQHandler(&huart1); // calls the STM32 HAL library callback function to handle the interrupt for USART1, which manages tasks like clearing interrupt flags and invoking any registered callbacks
}

void USART2_IRQHandler(void) {
  HAL_UART_IRQHandler(&huart2); // calls the STM32 HAL library callback function to handle the interrupt for USART2, which manages tasks like clearing interrupt flags and invoking any registered callbacks
}
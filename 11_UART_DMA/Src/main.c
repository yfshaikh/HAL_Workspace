#include "stm32f4xx_hal.h"
#include <stdint.h>


USART_HandleTypeDef huart1;
DMA_HandleTypeDef hdma_usart1_rx;
DMA_HandleTypeDef hdma_usart1_tx;
void uart_init(void);

uint8_t tx_buffer[10] = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100};
uint8_t rx_buffer[10];

/*
 * Main program entry point
 * Initializes UART peripheral and sets up DMA-driven transmission and reception
 * The while loop does nothing - all work is handled by DMA interrupt handlers
 */
int main() {
  HAL_Init();
  uart_init();
  
  HAL_UART_Transmit_DMA(&huart1, (uint8_t *)tx_buffer, sizeof(tx_buffer), HAL_MAX_DELAY);
  HAL_UART_Receive_DMA(&huart1, (uint8_t *)rx_buffer, sizeof(rx_buffer));

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

  // enable DMA clock access
  __HAL_RCC_DMA2_CLK_ENABLE();

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

  // configure usart1 rx dma
  hdma_usart1_rx.Instance = DMA2_Stream2; // select DMA2 Stream2 for USART1 RX data transfer. This stream is commonly used for USART1 RX on STM32F4 microcontrollers, allowing efficient data movement from the USART peripheral to memory without CPU intervention.
  hdma_usart1_rx.Init.Channel = DMA_CHANNEL_4; // select channel 4 for the DMA transfer. This channel is typically associated with USART1 RX in STM32F4 microcontrollers, enabling efficient data movement from the USART1 peripheral to memory without CPU intervention.
  hdma_usart1_rx.Init.Direction = DMA_PERIPH_TO_MEMORY; // set DMA transfer direction from peripheral to memory, which is appropriate for USART RX operations where data is received from the USART peripheral and stored in memory.
  hdma_usart1_rx.Init.PeriphInc = DMA_PINC_DISABLE; // disable peripheral address increment. This means that the peripheral address (USART data register) will remain constant during the DMA transfer, while the memory address will increment to store successive received bytes in a buffer.
  hdma_usart1_rx.Init.MemInc = DMA_MINC_ENABLE; // enable memory address increment. This allows the memory address to increment after each data transfer, which is necessary for storing multiple received bytes in a buffer.
  hdma_usart1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE; // set peripheral data alignment to byte (8 bits). This matches the typical data size for USART communication, ensuring proper data handling during the DMA transfer.
  hdma_usart1_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE; // set memory data alignment to byte (8 bits). This ensures that the data stored in memory is properly aligned for USART communication, which typically involves 8-bit data frames, allowing for efficient access and processing of the received data in memory.
  hdma_usart1_rx.Init.Mode = DMA_NORMAL; // set DMA mode to normal. This means that the DMA transfer will complete after transferring all the data in the buffer.
  hdma_usart1_rx.Init.Priority = DMA_PRIORITY_LOW; // set DMA priority to low. This determines the priority of the DMA transfer relative to other DMA streams. In this case, we set it to low since USART RX data transfers typically do not require high priority compared to other critical DMA operations in the system.
  hdma_usart1_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE; // disable FIFO mode for the DMA transfer. This means that the DMA will operate in direct mode, transferring data directly between the peripheral and memory without using an intermediate FIFO buffer, which is suitable for simple USART RX operations where data is received in a straightforward manner without the need for complex buffering.
  HAL_DMA_Init(&hdma_usart1_rx); // initialize the DMA with the specified settings for USART1 RX

  // link the DMA handle to the UART handle for RX operations
  huart1.hdmarx = &hdma_usart1_rx; // link the DMA handle to the UART handle for RX operations. This allows the HAL library to manage DMA transfers for USART1 RX using the specified DMA configuration, enabling efficient data reception without CPU intervention.

  // configure usart1 tx dma
  hdma_usart1_tx.Instance = DMA2_Stream7; // select DMA2 Stream7 for USART1 TX data transfer. This stream is commonly used for USART1 TX on STM32F4 microcontrollers, allowing efficient data movement from memory to the USART peripheral without CPU intervention.
  hdma_usart1_tx.Init.Channel = DMA_CHANNEL_4; // select channel 4 for the DMA transfer. This channel is typically associated with USART1 TX in STM32F4 microcontrollers, enabling efficient data movement from memory to the USART1 peripheral without CPU intervention.
  hdma_usart1_tx.Init.Direction = DMA_MEMORY_TO_PERIPH; // set DMA transfer direction from memory to peripheral, which is appropriate for USART TX operations where data is transmitted from memory to the USART peripheral.
  hdma_usart1_tx.Init.PeriphInc = DMA_PINC_DISABLE; // disable peripheral address increment. This means that the peripheral address (USART data register) will remain constant during the DMA transfer, while the memory address will increment to read successive bytes from a buffer for transmission.
  hdma_usart1_tx.Init.MemInc = DMA_MINC_ENABLE; // enable memory address increment. This allows the memory address to increment after each data transfer, which is necessary for reading multiple bytes from a buffer for transmission via USART.
  hdma_usart1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE; // set peripheral data alignment to byte (8 bits). This matches the typical data size for USART communication, ensuring proper data handling during the DMA transfer.
  hdma_usart1_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE; // set memory data alignment to byte (8 bits). This ensures that the data stored in memory is properly aligned for USART communication, which typically involves 8-bit data frames, allowing for efficient access and processing of the data to be transmitted in memory.
  hdma_usart1_tx.Init.Mode = DMA_NORMAL; // set DMA mode to normal. This means that the DMA transfer will complete after transferring all the data in the buffer.
  hdma_usart1_tx.Init.Priority = DMA_PRIORITY_LOW; // set DMA priority to low. This determines the priority of the DMA transfer relative to other DMA streams. In this case, we set it to low since USART TX data transfers typically do not require high priority compared to other critical DMA operations in the system.
  hdma_usart1_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE; // disable FIFO mode for the DMA transfer. This means that the DMA will operate in direct mode, transferring data directly between memory and the peripheral without using an intermediate FIFO buffer, which is suitable for simple USART TX operations where data is transmitted in a straightforward manner without the need for complex buffering.
  HAL_DMA_Init(&hdma_usart1_tx); // initialize the DMA with the specified settings for USART1 TX

  // link the DMA handle to the UART handle for TX operations
  huart1.hdmatx = &hdma_usart1_tx; // link the DMA handle to the UART handle for TX operations. This allows the HAL library to manage DMA transfers for USART1 TX using the specified DMA configuration, enabling efficient data transmission without CPU intervention.

  // dma stream2 irqn interrupt configuration
  HAL_NVIC_SetPriority(DMA2_Stream2_IRQn, 0, 0); // set DMA2 Stream2 interrupt priority. This configures the priority level for the interrupt generated by DMA2 Stream2, which is used for USART1 RX operations, allowing the CPU to respond to DMA transfer events such as completion or errors in a timely manner.
  HAL_NVIC_EnableIRQ(DMA2_Stream2_IRQn); // enable DMA2 Stream2 interrupt in the NVIC controller. This allows the CPU to respond to DMA transfer events, such as completion or errors, by invoking the corresponding interrupt service routine (ISR) when the DMA controller signals an


  // dma stream7 irqn interrupt configuration
  HAL_NVIC_SetPriority(DMA2_Stream7_IRQn, 0, 0); // set DMA2 Stream7 interrupt priority. This configures the priority level for the interrupt generated by DMA2 Stream7, which is used for USART1 TX operations, allowing the CPU to respond to DMA transfer events such as completion or errors in a timely manner.
  HAL_NVIC_EnableIRQ(DMA2_Stream7_IRQn); // enable DMA2 Stream7 interrupt in the NVIC controller. This allows the CPU to respond to DMA transfer events, such as completion or errors, by invoking the corresponding interrupt service routine (ISR) when the DMA controller signals an interrupt for Stream7, which is commonly
}


void DMA2_Stream2_IRQHandler(void) {
  HAL_DMA_IRQHandler(&hdma_usart1_rx); // calls the STM32 HAL library callback function to handle the interrupt for DMA2 Stream2, which manages tasks like clearing interrupt flags and invoking any registered callbacks for USART1 RX operations.
}

void DMA2_Stream7_IRQHandler(void) {
  HAL_DMA_IRQHandler(&hdma_usart1_tx); // calls the STM32 HAL library callback function to handle the interrupt for DMA2 Stream7, which manages tasks like clearing interrupt flags and invoking any registered callbacks for USART1 TX operations.
}
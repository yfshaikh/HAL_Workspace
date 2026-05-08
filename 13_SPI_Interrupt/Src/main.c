#include "stm32f4xx_hal.h"
#include <stdint.h>
#include <stdio.h>
#include "adc.h"
#include "uart.h"


SPI_HandleTypeDef hspi1;
void spi1_init(void);

uint8_t tx_buffer[10] = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100};
uint8_t rx_buffer[10];

int counter = 0;

void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi) {
  counter++;
}

int main() {
  HAL_Init();
  spi1_init();

  HAL_SPI_TransmitReceive_IT(&hspi1, tx_buffer, rx_buffer, sizeof(tx_buffer));

  while (1) {

  }
  
}

/*
 * ============================================================================
 * SPI PROTOCOL AND PIN CONFIGURATION
 * ============================================================================
 * 
 * SPI (Serial Peripheral Interface) is a synchronous communication protocol
 * for transferring data between microcontrollers and peripherals (SD cards,
 * sensors, displays, etc). It uses 4 signals (3 data + 1 chip select).
 * 
 * This project uses 3 pins in FULL-DUPLEX mode (send and receive simultaneously):
 * 
 * PA5 = SCK (Serial Clock)
 *    - Master generates clock signal to synchronize data transfer
 *    - Shared by master and slave
 *    - Direction: Master → Slave (one-way)
 * 
 * PA7 = MOSI (Master Out, Slave In)
 *    - Data sent FROM master TO slave
 *    - Direction: Master → Slave (one-way)
 *    - Example: Sending commands to SD card
 * 
 * PA6 = MISO (Master In, Slave Out)
 *    - Data received FROM slave TO master
 *    - Direction: Slave → Master (one-way)
 *    - Example: Reading data from sensor
 * 
 * SIMULTANEOUS TRANSFER:
 * - While master sends 8 bits on MOSI, it simultaneously receives 8 bits on MISO
 * - Both happen in sync with SCK clock pulses
 * - This is FULL-DUPLEX communication (both directions at same time)
 * 
 * ALTERNATE FUNCTION MODE (AF_PP):
 * - GPIO pins can be controlled by hardware peripherals instead of software
 * - AF5_SPI1 tells the microcontroller: "Route SPI1 signals to PA5/6/7"
 * - Without this, pins would be regular GPIO (can't do SPI)
 * - Push-Pull (PP) means pin can actively drive high AND low (needed for clocks)
 * 
 * ============================================================================
 */
void spi1_init(void) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  // enable SPI1 pins clock access
  __HAL_RCC_GPIOA_CLK_ENABLE();
  // enable SPI1 clock access
  __HAL_RCC_SPI1_CLK_ENABLE();

  // Configure PA5 (SCK), PA6 (MISO), and PA7 (MOSI) for SPI1
  GPIO_InitStruct.Pin = GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP; // Alternate Function Push-Pull: SPI peripheral controls these pins
  GPIO_InitStruct.Pull = GPIO_NOPULL; // No pull-up/down needed; SPI signals are actively driven
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH; // High speed for fast SPI clock
  GPIO_InitStruct.Alternate = GPIO_AF5_SPI1; // Route SPI1 to these pins
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  // Configure SPI1 as master with standard settings
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER; // This STM32 is the master (generates clock and initiates transfers)
  hspi1.Init.Direction = SPI_DIRECTION_2LINES; // Full-duplex: separate MOSI and MISO lines
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT; // Transfer 8 bits per clock pulse
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW; // Clock idles LOW between transfers
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE; // Sample data on first clock edge (rising edge)
  hspi1.Init.NSS = SPI_NSS_SOFT; // Chip Select (NSS) is controlled by software (not used in this simple example)
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2; // Clock speed = APB clock / 2 (fast SPI)
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB; // Send Most Significant Bit first
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE; // Disable TI mode (use standard Motorola SPI mode)
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE; // No CRC error checking
  hspi1.Init.CRCPolynomial = 10; // CRC polynomial (unused since CRC disabled)
  HAL_SPI_Init(&hspi1);

  HAL_NVIC_SetPriority(SPI1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(SPI1_IRQn);

}

void SPI1_IRQHandler(void){
  HAL_SPI_IRQHandler(&hspi1); // calls the STM32 HAL library callback function to handle the interrupt for SPI1, which manages tasks like clearing interrupt flags and invoking any registered callbacks for SPI events such as transfer complete or errors.
}

void SysTick_Handler(void) {
  // whenever there is a SysTick interrupt, this function will be called. 
  // This function is defined weakly in the HAL library, so we can override it here to provide our own implementation. 
  // In this case, we will just call the HAL_IncTick() function to increment the tick count, which is used for timing functions in the HAL library.
  // a SysTick is a timer that generates an interrupt at regular intervals, which can be used for timing and scheduling tasks in an embedded system.
  HAL_IncTick();
}




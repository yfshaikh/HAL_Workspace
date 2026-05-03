#include "stm32f4xx_hal.h"

#define BTN_PIN GPIO_PIN_13
#define BTN_PORT GPIOC
#define LED_PIN GPIO_PIN_5
#define LED_PORT GPIOA

// BTN = PC13, BUS = AHB1EN bit0
// LED = PA5, BUS = AHB1EN bit2


void pc13_btn_init(void);
void pa5_led_init(void);



uint8_t buttonStatus;

int main() {
  HAL_Init();
  pc13_btn_init();
  pa5_led_init();

  while (1) {
    buttonStatus = HAL_GPIO_ReadPin(BTN_PORT, BTN_PIN);
    HAL_GPIO_WritePin(LED_PORT, LED_PIN, buttonStatus);
  }

  
}


void pc13_btn_init(void) {
  // configure PC13 as input pin
  __HAL_RCC_GPIOC_CLK_ENABLE();

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = BTN_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

void pa5_led_init(void) {
  // configure PA5 as output pin
  __HAL_RCC_GPIOA_CLK_ENABLE();

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = LED_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

  // initialize GPIOA port with the configuration specified in GPIO_InitStruct
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}


void SysTick_Handler(void) {
  // whenever there is a SysTick interrupt, this function will be called. 
  // This function is defined weakly in the HAL library, so we can override it here to provide our own implementation. 
  // In this case, we will just call the HAL_IncTick() function to increment the tick count, which is used for timing functions in the HAL library.
  // a SysTick is a timer that generates an interrupt at regular intervals, which can be used for timing and scheduling tasks in an embedded system.
  HAL_IncTick();
}
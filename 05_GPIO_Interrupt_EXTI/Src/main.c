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
  gpio_pc13_interrupt_init();


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

void gpio_pc13_interrupt_init(void){
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  __HAL_RCC_GPIOC_CLK_ENABLE();

  // configure the GPIO pin for the button as an input with an interrupt on the rising edge.
  GPIO_InitStruct.Pin = BTN_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING; // interrupt on rising edge
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

  HAL_GPIO_Init(BTN_PORT, &GPIO_InitStruct);

  // once we initialize the GPIO pin, we can reuse the InitStruct for the LED pin by just changing the relevant fields.
  GPIO_InitStruct.Pin = LED_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; // output push-pull mode
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

  HAL_GPIO_Init(LED_PORT, &GPIO_InitStruct);

  // configure EXTI
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0); // set the priority of the EXTI interrupt (also 0 for preemption priority and 0 for subpriority)
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn); // enable the EXTI interrupt in the NVIC (Nested Vectored Interrupt Controller)
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
  HAL_GPIO_TogglePin(LED_PORT, LED_PIN);
}


// this is the interrupt handler for the EXTI line 15 to 10, which includes pin 13.
// it will be called whenever there is an interrupt on any of the pins in that range, but we will check if it was pin 13 that caused the interrupt.
void EXTI15_10_IRQHandler(void) {
  HAL_GPIO_EXTI_IRQHandler(BTN_PIN); // this will call the callback function HAL_GPIO_EXTI_Callback() if the interrupt was caused by pin 13.
}


#include "stm32f4xx_hal.h"


extern ADC_HandleTypeDef hadc1;

static void adc_pa0_continuous_conversion_init(void);

uint32_t p0_adc_read(void) {
    return HAL_ADC_GetValue(&hadc1); // this function returns the converted value of the ADC channel. it reads the value from the ADC data register and returns it as a 32-bit unsigned integer.
}

void adc_init_start(void) {
    adc_pa0_continuous_conversion_init(); // this function initializes the ADC peripheral and configures it for continuous conversion mode on channel 0 (PA0). it sets up the necessary GPIO pin, ADC settings, and starts the ADC conversion process.
    HAL_ADC_Start(&hadc1); // this function starts the ADC conversion process. it sets the appropriate bits in the ADC control register to begin the conversion of the selected channel.
}

void adc_pa0_continuous_conversion_init(void) {
    // we will use PA0 as the ADC input pin, which corresponds to ADC1_1 (first channel of ADC1)

    ADC_ChannelConfTypeDef sConfig = {0};
    GPIO_InitTypeDef GPIO_InitStruct = {0};


    // configure PA0 as an analog input pin
    __HAL_RCC_GPIOA_CLK_ENABLE(); // enable clock access to GPIOA
    GPIO_InitStruct.Pin = GPIO_PIN_0; // PA0
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG; // analog mode
    GPIO_InitStruct.Pull = GPIO_NOPULL; // no pull-up or pull-down resistors
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);



    // configure ADC module for continuous conversion
    __HAL_RCC_ADC1_CLK_ENABLE(); // enable clock access to ADC1

    hadc1.Instance = ADC1; // use ADC1
    hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2; // set ADC clock prescaler
    hadc1.Init.Resolution = ADC_RESOLUTION_12B; // set ADC resolution to 12 bits
    hadc1.Init.ContinuousConvMode = ENABLE; // enable continuous conversion mode for the ADC, which allows it to automatically start a new conversion after the previous one finishes without needing to be triggered again by software or hardware.
    hadc1.Init.DiscontinuousConvMode = DISABLE; // disable discontinuous mode
    hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE; // no external trigger
    hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START; // start conversion by software
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT; // right align the ADC data
    hadc1.Init.NbrOfConversion = 1; // we will only convert one channel
    hadc1.Init.DMAContinuousRequests = DISABLE; // disable DMA continuous requests
    hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONVERSION; // end of conversion flag is set after each conversion
    HAL_ADC_Init(&hadc1); // initialize the ADC with the specified settings

    // configure the ADC channel
    sConfig.Channel = ADC_CHANNEL_0; // select channel 0 (PA0)
    sConfig.Rank = 1; // rank of the channel in the regular group sequence
    sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES; // set sampling time
    HAL_ADC_ConfigChannel(&hadc1, &sConfig); // configure the ADC channel with the specified settings

    // enable ADC interrupt
    HAL_NVIC_SetPriority(ADC_IRQn, 0, 0); // set ADC interrupt priority
    HAL_NVIC_EnableIRQ(ADC_IRQn); // enable ADC interrupt in the NVIC
}


void adc_pa0_interrupt_init(void) {
    // we will use PA0 as the ADC input pin, which corresponds to ADC1_1 (first channel of ADC1)

    ADC_ChannelConfTypeDef sConfig = {0};
    GPIO_InitTypeDef GPIO_InitStruct = {0};


    // configure PA0 as an analog input pin
    __HAL_RCC_GPIOA_CLK_ENABLE(); // enable clock access to GPIOA
    GPIO_InitStruct.Pin = GPIO_PIN_0; // PA0
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG; // analog mode
    GPIO_InitStruct.Pull = GPIO_NOPULL; // no pull-up or pull-down resistors
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);



    // configure ADC module for continuous conversion
    __HAL_RCC_ADC1_CLK_ENABLE(); // enable clock access to ADC1

    hadc1.Instance = ADC1; // use ADC1
    hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2; // set ADC clock prescaler
    hadc1.Init.Resolution = ADC_RESOLUTION_12B; // set ADC resolution to 12 bits
    hadc1.Init.ContinuousConvMode = DISABLE; // enable continuous conversion mode
    hadc1.Init.DiscontinuousConvMode = DISABLE; // disable discontinuous mode
    hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE; // no external trigger
    hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START; // start conversion by software
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT; // right align the ADC data
    hadc1.Init.NbrOfConversion = 1; // we will only convert one channel
    hadc1.Init.DMAContinuousRequests = DISABLE; // disable DMA continuous requests
    hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONVERSION; // end of conversion flag is set after each conversion
    HAL_ADC_Init(&hadc1); // initialize the ADC with the specified settings

    // configure the ADC channel
    sConfig.Channel = ADC_CHANNEL_0; // select channel 0 (PA0)
    sConfig.Rank = 1; // rank of the channel in the regular group sequence
    sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES; // set sampling time
    HAL_ADC_ConfigChannel(&hadc1, &sConfig); // configure the ADC channel with the specified settings
}


/*
 * ============================================================================
 * INTERRUPT FLOW EXPLANATION
 * ============================================================================
 * 
 * When an ADC conversion completes, the following sequence happens:
 * 
 * 1. HARDWARE EVENT
 *    - ADC finishes converting PA0 and sets the interrupt flag
 * 
 * 2. CPU RECOGNIZES INTERRUPT
 *    - CPU stops current execution and jumps to the address in the interrupt 
 *      vector table for ADC1
 *    - Vector table points to ADC_IRQHandler() below
 * 
 * 3. ADC_IRQHandler() [THIS FUNCTION - adc.c]
 *    - Automatically called by the CPU when interrupt fires
 *    - This is your custom interrupt service routine (ISR) entry point
 *    - Set up in the interrupt vector table by startup/linker code
 *    - PURPOSE: Provides a fixed entry point that you control and can 
 *      customize (e.g., add logging, timing measurements, debug logic, etc.)
 *    - WHY NOT CALL HAL DIRECTLY? The CPU hardware needs a fixed address 
 *      in the vector table. By having this as your entry point, you can:
 *      * Add custom pre/post-processing logic around HAL calls
 *      * Swap or update library versions independently
 *      * Debug ISR entry and execution patterns
 *    - ACTION: Must call HAL_ADC_IRQHandler() - this is REQUIRED, not optional
 * 
 * 4. HAL_ADC_IRQHandler() [HAL LIBRARY]
 *    - Does the real hardware abstraction work:
 *      * Clears the interrupt flag from ADC hardware
 *      * Checks conversion status (complete, error, overrun, etc)
 *      * Updates the ADC handle structure
 *    - Determines what happened and calls appropriate callback:
 *      * If conversion completed: HAL_ADC_ConvCpltCallback()
 *      * If error occurred: HAL_ADC_ErrorCallback()
 *      * If DMA transfer complete: HAL_ADC_ConvHalfCpltCallback()
 * 
 * 5. HAL_ADC_ConvCpltCallback() [main.c - defined by you]
 *    - Automatically called by HAL_ADC_IRQHandler() when conversion completes
 *    - This is a WEAK function in the HAL library that you override
 *    - If you don't define it, the HAL library provides an empty stub
 *    - PURPOSE: Your application-specific business logic runs here
 *    - In our case: Read ADC value and printf it to console
 * 
 * 6. RETURN FROM INTERRUPT
 *    - Control returns: HAL_ADC_IRQHandler() → ADC_IRQHandler() → CPU
 *    - CPU resumes the main program (while loop in main.c)
 * 
 * ============================================================================
 */
void ADC_IRQHandler(void) {
    HAL_ADC_IRQHandler(&hadc1); // calls the STM32 HAL library callback function to handle the interrupt for ADC1, which manages tasks like clearing interrupt flags and invoking any registered callbacks
}
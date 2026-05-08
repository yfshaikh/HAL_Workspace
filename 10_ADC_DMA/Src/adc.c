#include "stm32f4xx_hal.h"


extern ADC_HandleTypeDef hadc1;

DMA_HandleTypeDef hdma_adc1; // this is the DMA handle structure that will be used to configure and manage the DMA transfer for ADC1. It holds all the necessary information about the DMA stream, channel, data direction, buffer addresses, transfer size, and other settings required to set up and control the DMA operation for ADC data transfers.

uint32_t p0_adc_read(void) {
    return HAL_ADC_GetValue(&hadc1);
}

/*
 * ============================================================================
 * ADC DMA MODE
 * ============================================================================
 * 
 * This project uses DMA-assisted ADC conversion for efficient data transfer.
 * 
 * FLOW:
 * 1. ADC converts PA0
 * 2. Conversion complete → DMA automatically transfers data to memory buffer
 * 3. DMA transfer complete → DMA interrupt fires
 * 4. CPU processes the buffered data in DMA interrupt handler
 * 
 * KEY BENEFITS:
 * - CPU is NOT involved in moving data from ADC to memory
 * - DMA handles all transfers autonomously
 * - CPU can do other work while DMA transfers
 * - Most efficient for continuous data acquisition
 * 
 * ============================================================================
 * UNDERSTANDING HANDLERS vs INIT STRUCTS
 * ============================================================================
 * 
 * INIT STRUCTS (e.g., hadc1.Init, hdma_adc1.Init):
 *    - Configuration data written during initialization
 *    - Example: hadc1.Init.Resolution = ADC_RESOLUTION_12B;
 *    - Think of them as "blueprints" for hardware setup
 * 
 * HANDLERS (e.g., DMA2_Stream0_IRQHandler):
 *    - Functions executed when interrupts fire at runtime
 *    - Called by CPU when hardware events occur
 *    - Should execute quickly
 *    - Think of them as "event responders"
 * 
 * ============================================================================
 * DMA INTERRUPT FLOW
 * ============================================================================
 * 
 * 1. ADC finishes converting PA0
 * 2. DMA automatically transfers data from ADC register to memory buffer
 * 3. DMA transfer complete → DMA sets completion flag and triggers interrupt
 * 4. CPU jumps to DMA2_Stream0_IRQHandler (vector table)
 * 5. DMA2_Stream0_IRQHandler calls HAL_DMA_IRQHandler(hadc1.DMA_Handle)
 * 6. HAL clears flags and calls HAL_DMA_XferCpltCallback (if defined)
 * 7. Your callback processes the data from the buffer
 * 8. Control returns to main program (while loop)
 * 
 * ADC-DMA LINK:
 *    - __HAL_LINKDMA(&hadc1, DMA_Handle, hdma_adc1) connects them
 *    - hadc1.DMA_Handle now points to hdma_adc1
 *    - When ADC completes, it triggers DMA transfer automatically
 * 
 * ============================================================================
 */
void adc_pa0_dma_init(void) {
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
    hadc1.Init.DMAContinuousRequests = ENABLE; // enable DMA continuous requests
    hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONVERSION; // end of conversion flag is set after each conversion
    HAL_ADC_Init(&hadc1); // initialize the ADC with the specified settings

    // configure the ADC channel
    sConfig.Channel = ADC_CHANNEL_0; // select channel 0 (PA0)
    sConfig.Rank = 1; // rank of the channel in the regular group sequence
    sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES; // set sampling time
    HAL_ADC_ConfigChannel(&hadc1, &sConfig); // configure the ADC channel with the specified settings

    // if we want to use DMA for ADC, we need to enable clock to DMA2
    __HAL__RCC_DMA2_CLK_ENABLE(); // enable clock access to DMA2, which is the DMA controller that can be used for ADC1 on STM32F4 series microcontrollers. This allows us to set up DMA transfers for ADC data without CPU intervention, improving efficiency and performance when handling ADC conversions.


    // we need to enable the interrupt for stream0
    // there is also an associated interrupt handler for this stream
    HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 0); // set DMA2 Stream0 interrupt priority
    HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn); // enable DMA2 Stream0 interrupt in the NVIC controller. This allows the CPU to respond to DMA transfer events, such as completion or errors, by invoking the corresponding interrupt service routine (ISR) when the DMA controller signals an interrupt for Stream0, which is commonly used for ADC data transfers in STM32F4 microcontrollers
    
    hdma_adc1.Instance = DMA2_Stream0; // select DMA2 Stream0 for ADC1 data transfer. This stream is commonly used for ADC1 on STM32F4 microcontrollers, allowing efficient data movement from the ADC peripheral to memory without CPU intervention.
    hdma_adc1.Init.Channel = DMA_CHANNEL_0; // select channel 0 for the DMA transfer. This channel is typically associated with ADC1 in STM32F4 microcontrollers,
    hdma_adc1.Init.Direction = DMA_MEMORY_TO_PERIPH; // set data transfer direction from memory to peripheral. This means that the DMA will read data from a memory buffer and write it to the ADC data register, which is useful for feeding data into the ADC for conversion or for handling ADC results in a circular buffer.
    hdma_adc1.Init.PeriphInc = DMA_PINC_DISABLE; // disable peripheral address increment. This means that the peripheral address (ADC data register) will remain constant during the DMA transfer, while the memory address will increment to store successive ADC conversion results in a buffer.
    hdma_adc1.Init.MemInc = DMA_MINC_ENABLE; // enable memory address increment. This allows the memory address to increment after each data transfer, which is necessary for storing multiple ADC conversion results in a buffer.
    hdma_adc1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD; // set peripheral data alignment to half-word (16 bits). This matches the 12-bit resolution of the ADC, which is typically stored in a 16-bit register, ensuring proper data handling during the DMA transfer.
    hdma_adc1.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD; // set memory data alignment to half-word (16 bits). This ensures that the data stored in memory is properly aligned for the ADC conversion results, which are typically 16 bits in size, allowing for efficient access and processing of the ADC data in memory.
    hdma_adc1.Init.Mode = DMA_CIRCULAR; // set DMA mode to circular. This means that when the DMA transfer reaches the end of the buffer, it will automatically wrap around to the beginning and continue transferring data, allowing for continuous data acquisition from the ADC without needing to restart the DMA transfer manually.
    hdma_adc1.Init.Priority = DMA_PRIORITY_LOW; // set DMA priority to low. This determines the priority of the DMA transfer relative to other DMA streams. In this case, we set it to low since ADC data transfers typically do not require high priority compared to other critical DMA operations in the system.
    hdma_adc1.Init.FIFOMode = DMA_FIFOMODE_DISABLE; // disable FIFO mode. This means that the DMA will operate in direct mode without using the FIFO buffer, which is suitable for simple memory-to-peripheral transfers like ADC data, where the data size is small and does not require the additional buffering provided by the FIFO.
    HAL_DMA_Init(&hdma_adc1); // initialize the DMA with the specified settings. This function configures the DMA controller according to the parameters set in the hdma_adc1 structure, preparing it for handling ADC data transfers efficiently without CPU intervention.
    __HAL_LINKDMA(&hadc1, DMA_Handle, hdma_adc1); // link the DMA handle to the ADC handle. This macro connects the DMA handle (hdma_adc1) to the ADC handle (hadc1) by setting the DMA_Handle member of the ADC handle to point to the DMA handle. This allows the ADC and DMA to work together seamlessly, enabling the ADC to trigger DMA transfers for data acquisition without needing additional configuration or management in the main application code, simplifying the integration of ADC and DMA for efficient data handling.
}

void DMA2_Stream0_IRQHandler(void) {
    HAL_DMA_IRQHandler(hadc1.DMA_Handle); // calls the STM32 HAL library callback function to handle the interrupt for DMA2 Stream0, which manages tasks like clearing interrupt flags and invoking any registered callbacks related to the DMA transfer associated with ADC1.
}
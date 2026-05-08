#include "stm32f4xx_hal.h"
#include <stdint.h>
#include <stdio.h>

#define DEVICE_ADDRESS 0x53 << 1 // I2C slave device address (shifted left for 7-bit addressing). This is the unique address assigned to the I2C slave device we want to communicate with. The << 1 is used because the HAL library expects the address in a specific format where the least significant bit is reserved for read/write operations.

#define DEVID_R 0x00 // device ID register address for the I2C slave device we want to read from. This is the register we will read to verify communication with the slave device.
#define POWER_CTL_R 0x2D // power control register address for the I2C slave device. This is the register we will write to in order to configure the power settings of the slave device.
#define DATA_FORMAT_R 0x31 // data format register address for the I2C slave device. This is the register we will write to in order to configure the data format settings of the slave device.

#define FOUR_G_RANGE 0x01 // value to set the data format register to configure the slave device for a +/- 4g measurement range. This is a specific configuration value that tells the slave device how to interpret the data it will be sending back.
#define RESET_ALL_BITS 0x00 // value to reset all bits in a register. This can be used to clear any existing settings in a register before configuring it with new settings.
#define SET_MEASURE_BIT 0x08 // value to set the measure bit in the power control register. This is used to enable measurement mode on the slave device, allowing it to start taking measurements and sending data back when requested.

#define DATA_START_ADDR 0x32 // starting address of the data registers in the slave device. This is the register address from which we will start reading measurement data from the slave device.

#define FOUR_G_SCALE_FACTOR 4.0 / 512.0 // scale factor to convert raw ADC values to g's for a +/- 4g range. This is calculated based on the resolution of the ADC and the configured measurement range, allowing us to convert the raw data from the slave device into meaningful physical units (g's).


I2C_HandleTypeDef hi2c1;
void i2c1_init(void);
void adxl_write(uint8_t reg, uint8_t data);
void adxl_read(uint8_t reg);
void adxl_init(void);

uint8_t rx_buffer[6];
uint8_t device_id;

int16_t x, y, z;
float x_g, y_g, z_g;


int main() {
  HAL_Init();
  i2c1_init();
  adxl_init();


  while (1) {
    adx1_read_values(DATA_START_ADDR); // read the measurement data from the slave device starting at the specified data register address. This will fill the rx_buffer with the latest measurement data from the slave device, which we can then process as needed.

    x = (rx_buffer[1] << 8) | rx_buffer[0]; // combine the two bytes for the X-axis measurement into a single 16-bit signed integer. The first byte (rx_buffer[0]) is the least significant byte, and the second byte (rx_buffer[1]) is the most significant byte. We shift the most significant byte left by 8 bits and then use a bitwise OR to combine it with the least significant byte.
    y = (rx_buffer[3] << 8) | rx_buffer[2]; // combine the two bytes for the Y-axis measurement into a single 16-bit signed integer using the same method as for the X-axis.
    z = (rx_buffer[5] << 8) | rx_buffer[4]; // combine the two bytes for the Z-axis measurement into a single 16-bit signed integer using the same method as for the X and Y axes.

    x_g = x * FOUR_G_SCALE_FACTOR; // convert the raw X-axis measurement to g's using the scale factor. This gives us the acceleration in g's for the X-axis based on the raw data from the slave device.
    y_g = y * FOUR_G_SCALE_FACTOR; // convert the raw Y-axis measurement to g's using the scale factor. This gives us the acceleration in g's for the Y-axis based on the raw data from the slave device.
    z_g = z * FOUR_G_SCALE_FACTOR; // convert the raw Z-axis measurement to g's using the scale factor. This gives us the acceleration

    HAL_Delay(500); // add a delay to slow down the loop and make it easier to observe the output values. This is especially useful when debugging or monitoring the measurements, as it prevents the data from updating too rapidly.
  }
  
}


void adxl_write(uint8_t reg, uint8_t data) {
  uint8_t buffer[2] = {reg, data}; // create a buffer to hold the register address and the data byte we want to write. The first byte is the register address, and the second byte is the data we want to write to that register.
  HAL_I2C_Master_Transmit(&hi2c1, DEVICE_ADDRESS, buffer, sizeof(buffer), HAL_MAX_DELAY); // call the HAL function to transmit the data over I2C. This function takes the I2C handle, the device address, a pointer to the data buffer, the size of the buffer, and a timeout value. It will send the register address followed by the data byte to the slave device.
}

void adxl_read_values(uint8_t reg) {
  HAL_I2C_Mem_Read(&hi2c1, DEVICE_ADDRESS, reg, 1, (uint8_t*)rx_buffer, 6, HAL_MAX_DELAY);
}

void adxl_read_address(uint8_t reg) {
  HAL_I2C_Mem_Read(&hi2c1, DEVICE_ADDRESS, reg, 1, &device_id, 6, HAL_MAX_DELAY);
}

void adxl_init(void) {

  // read device ID to verify communication
  adxl_read_address(DEVID_R); // read the device ID register from the slave device and store the result in the device_id variable. This is a common step to verify that we can successfully communicate with the slave device before proceeding with further configuration.

  // set data format to +/- 4g
  adxl_write(DATA_FORMAT_R, FOUR_G_RANGE); // write the value defined by FOUR_G_RANGE to the data format register of the slave device. This configures the device to use a measurement range of +/- 4g, which is important for interpreting the data it will send back correctly.

  // reset all bits
  adxl_write(POWER_CTL_R, RESET_ALL_BITS); // write the value defined by RESET_ALL_BITS to the power control register of the slave device. This clears any existing settings in the power control register, ensuring that we start with a known state before enabling measurement mode.

  // config the PWR control
  adxl1_write(POWER_CTL_R, SET_MEASURE_BIT); // write the value defined by SET_MEASURE_BIT to the power control register of the slave device. This sets the measure bit, which enables measurement mode on the device, allowing it to start taking measurements and sending data back when requested.


}


void i2c1_init(void) {

  // PB8 => I2C1_SCL, PB9 => I2C1_SDA
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  // enable GPIOB clock access
  __HAL_RCC_GPIOB_CLK_ENABLE();

  // enable I2C1 clock access
  __HAL_RCC_I2C1_CLK_ENABLE();

  // configure PB8 and PB9 for I2C1 alternate function
  GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9; // PB8 = SCL, PB9 = SDA. the | operator is used to combine the two pin numbers into a single value that can be passed to the HAL_GPIO_Init() function.
  GPIO_InitStruct.Mode = GPIO_MODE_AF_OD; // alternate function open-drain mode. This means that the pin will be controlled by the alternate function (in this case, the I2C peripheral) and will be in open-drain mode, which allows it to only drive low output levels (sinking current) and requires an external pull-up resistor to drive the line high.
  GPIO_InitStruct.Pull = GPIO_PULLUP; // enable internal pull-up resistors on the I2C lines. This is necessary because I2C uses open-drain outputs, which can only pull the line low. The pull-up resistors ensure that the lines are pulled high when not being driven low by any device, allowing for proper communication on the I2C bus.
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH; // set GPIO speed to very high. This configures the slew rate of the GPIO pins, allowing for faster signal transitions which is beneficial for high-speed I2C communication.
  GPIO_InitStruct.Alternate = GPIO_AF4_I2C1; // alternate function 4 is the I2C1 peripheral. This tells the microcontroller to route the I2C1 signals to these pins.
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  // configure I2C1 peripheral
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000; // set I2C clock speed to 100 kHz (standard mode)
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2; // set duty cycle for fast mode (not used in standard mode)
  hi2c1.Init.OwnAddress1 = 0; // set own address (not used in master mode)
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT; // use 7-bit addressing mode
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE; // disable dual address mode
  hi2c1.Init.OwnAddress2 = 0; // set second own address (not used)
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE; // disable general call mode
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE; // disable clock stretching
  HAL_I2C_Init(&hi2c1); // initialize the I2C peripheral with the specified settings

}


void SysTick_Handler(void) {
  // whenever there is a SysTick interrupt, this function will be called. 
  // This function is defined weakly in the HAL library, so we can override it here to provide our own implementation. 
  // In this case, we will just call the HAL_IncTick() function to increment the tick count, which is used for timing functions in the HAL library.
  // a SysTick is a timer that generates an interrupt at regular intervals, which can be used for timing and scheduling tasks in an embedded system.
  HAL_IncTick();
}




#pragma once

#define SSD1306_I2C_ADDR 0x3C

int init_i2c_ssd1306(void);
void cleanup_i2c_ssd1306(void);
void i2c_write_byte_ssd1306(uint8_t Cmd, uint8_t value);
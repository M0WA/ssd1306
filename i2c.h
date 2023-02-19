#pragma once

int init_i2c_ssd1306(uint8_t i2c_addr);
void cleanup_i2c_ssd1306(void);
void i2c_write_byte_ssd1306(uint8_t Cmd, uint8_t value);
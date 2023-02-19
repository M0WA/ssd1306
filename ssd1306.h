#pragma once

int init_ssd1306(uint8_t i2c_addr);
void cleanup_ssd1306(void);

void clear_ssd1306(void);
void set_string_ssd1306(unsigned char *str, size_t len);
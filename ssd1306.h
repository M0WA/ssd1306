#pragma once

int init_ssd1306(void);
void cleanup_ssd1306(void);

void clear_ssd1306(void);
void set_string_ssd1306(unsigned char *str, size_t len);
#include <linux/kernel.h>
#include<linux/string.h>

#include "ssd1306.h"
#include "i2c.h"
#include "characters.h"

#define OLED_WIDTH    128
#define OLED_HEIGHT   32
#define OLED_Page	  ( OLED_HEIGHT / 8 )

#define IIC_CMD        0X00
#define IIC_RAM        0X40

#define FONT_PADDING_RIGHT 2 //pixel

#define OLED_BACKBUF_SIZE ( OLED_WIDTH * OLED_HEIGHT / 8 )
#define OLED_CLEAR_BACKBUFFER() \
    memset(oled_backbuffer, 0x00, sizeof(oled_backbuffer));
static unsigned char oled_backbuffer[OLED_BACKBUF_SIZE];


static void oled_write_reg(char data)
{
    i2c_write_byte_ssd1306(IIC_CMD, data);
}

static void oled_write_data(char data)
{
    i2c_write_byte_ssd1306(IIC_RAM, data);
}

static int init_oled_display(void)
{
	oled_write_reg(0xAE);

	oled_write_reg(0x40);//---set low column address
	oled_write_reg(0xB0);//---set high column address

	oled_write_reg(0xC8);//-not offset

	oled_write_reg(0x81);
	oled_write_reg(0xff);

	oled_write_reg(0xa1);

	oled_write_reg(0xa6);

	oled_write_reg(0xa8);
	oled_write_reg(0x1f);

	oled_write_reg(0xd3);
	oled_write_reg(0x00);

	oled_write_reg(0xd5);
	oled_write_reg(0xf0);

	oled_write_reg(0xd9);
	oled_write_reg(0x22);

	oled_write_reg(0xda);
	oled_write_reg(0x02);

	oled_write_reg(0xdb);
	oled_write_reg(0x49);

	oled_write_reg(0x8d);
	oled_write_reg(0x14);
	
	oled_write_reg(0xaf);
    return 0;
}

static void oled_commit_backbuffer(void)
{
    char Column,Page;
    unsigned char *ptr = oled_backbuffer;
    for(Page = 0; Page < OLED_Page; Page++) {
        oled_write_reg(0xb0 + Page);
        oled_write_reg(0x00);
        oled_write_reg(0x10);
        for(Column = 0; Column < OLED_WIDTH; Column++) {
            oled_write_data(*ptr);
            ptr++;
        }
    } 
}

/*
static void oled_backbuffer_set_pixel(int x, int y, unsigned char col)
{
    int pos = x + y / 8 * OLED_WIDTH;
	if(col) {
		oled_backbuffer[pos] |= (0x01 << (y % 8));
	} else {
		oled_backbuffer[pos] |= (0x00 << (y % 8));
	}
}
*/

static size_t oled_print_char(unsigned char c, unsigned char *buf)
{
    if(c=='\n') {
        c = ' ';
    }

    /*
    ** In our font array (SSD1306_font), space starts in 0th index.
    ** But in ASCII table, Space starts from 32 (0x20).
    ** So we need to match the ASCII table with our font table.
    ** We can subtract 32 (0x20) in order to match with our font table.
    */
    c -= 0x20;  //or c -= ' ';

    memcpy(buf, ssd1306_font[c], SSD1306_FONT_SIZE);
    return SSD1306_FONT_SIZE + FONT_PADDING_RIGHT;
}

static void oled_set_cursor_pixel_line(char x, char line)
{
    /* Move the Cursor to specified position only if it is in range */
    if((line <= OLED_Page) && (x < OLED_WIDTH))
    {
        oled_write_reg(0x21);         // cmd for the column start and end address
        oled_write_reg(x);            // column start addr
        oled_write_reg(OLED_WIDTH-1); // column end addr

        oled_write_reg(0x22);              // cmd for the page start and end address
        oled_write_reg(line);              // page start addr
        oled_write_reg(OLED_Page);         // page end addr
    }
}

int init_ssd1306(void)
{
    init_i2c_ssd1306();
    init_oled_display();
    clear_ssd1306();
    set_string_ssd1306("hello", 5);
    return 0;
}

void clear_ssd1306(void)
{
    OLED_CLEAR_BACKBUFFER()
    oled_commit_backbuffer();
    oled_set_cursor_pixel_line(0, 0);
}

void cleanup_ssd1306(void)
{
    clear_ssd1306();
    oled_write_reg(0xAE); // Entire Display OFF
    cleanup_i2c_ssd1306();
}

void set_string_ssd1306(unsigned char *str, size_t len)
{
    size_t i;
    unsigned char *buf = oled_backbuffer;
    OLED_CLEAR_BACKBUFFER()
    for(i = 0; i < len; i++) {        
        buf += oled_print_char(str[i], buf);
    }
    oled_set_cursor_pixel_line(0, 0);
    oled_commit_backbuffer();
}
#include <linux/i2c.h>

#include "i2c.h"

#define SSD1306_USE_I2C_BUS 1
#define SSD1306_SLAVE_DEVICE_NAME "ssd1306oled"

static struct i2c_adapter *i2c_adapter     = NULL;
static struct i2c_client  *i2c_client_oled = NULL;

static int oled_probe(struct i2c_client *client,
                          const struct i2c_device_id *id)
{
    pr_info("OLED Probed!!!\n");
    return 0;
}

static int oled_remove(struct i2c_client *client)
{
    pr_info("OLED Removed!!!\n");
    return 0;
}

static const struct i2c_device_id oled_id[] = {
    { SSD1306_SLAVE_DEVICE_NAME, 0 },
    { }
};
MODULE_DEVICE_TABLE(i2c, oled_id);

static struct i2c_driver oled_driver = {
    .driver = {
        .name   = SSD1306_SLAVE_DEVICE_NAME,
        .owner  = THIS_MODULE,
    },
    .probe          = oled_probe,
    .remove         = oled_remove,
    .id_table       = oled_id,
};

static struct i2c_board_info oled_i2c_board_info = {
    I2C_BOARD_INFO(SSD1306_SLAVE_DEVICE_NAME, SSD1306_I2C_ADDR)
};

int init_i2c_ssd1306(void)
{
    int ret = -1;
    i2c_adapter = i2c_get_adapter(SSD1306_USE_I2C_BUS);

    if( i2c_adapter != NULL )
    {
        i2c_client_oled = i2c_new_client_device(i2c_adapter, &oled_i2c_board_info);

        if( i2c_client_oled != NULL )
        {
            i2c_add_driver(&oled_driver);
            ret = 0;
        }
        
        i2c_put_adapter(i2c_adapter);
    }
    return ret;
}

void cleanup_i2c_ssd1306(void)
{
    i2c_unregister_device(i2c_client_oled);
    i2c_del_driver(&oled_driver);
}

void i2c_write_byte_ssd1306(uint8_t Cmd, uint8_t value)
{
    unsigned char buf[2] = {Cmd, value};
    if(i2c_client_oled) 
        i2c_master_send(i2c_client_oled, buf, 2);
}
#include <linux/module.h>
#include <linux/moduleparam.h>

#include "ssd1306.h"
#include "chardev.h"

static int i2c_addr = 0x3C;

int __init _driver_init(void)
{
  int ret = -1;
  ret = init_ssd1306(i2c_addr);
  init_chardev();
  return ret;
}

void __exit _driver_exit(void)
{
  cleanup_chardev();
  cleanup_ssd1306();
}

module_init(_driver_init);
module_exit(_driver_exit);
module_param(i2c_addr,int,0660);
MODULE_PARM_DESC(i2c_addr, "Adress of SSD1306 on I2C bus (default = 0x3C)");

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Moritz Wagner <moritzwag@gmail.com>");
MODULE_DESCRIPTION("SSD1306 I2C Driver using character device");
MODULE_VERSION("0.1");
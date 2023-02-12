#include <linux/module.h>

#include "ssd1306.h"
#include "chardev.h"

int __init _driver_init(void)
{
  int ret = -1;
  ret = init_ssd1306();
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
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("EmbeTronicX <embetronicx@gmail.com>");
MODULE_DESCRIPTION("SSD1306 I2C Driver");
MODULE_VERSION("1.40");
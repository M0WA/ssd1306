#include <linux/init.h>           // Macros used to mark up functions e.g. __init __exit
#include <linux/module.h>         // Core header for loading LKMs into the kernel
#include <linux/device.h>         // Header to support the kernel Driver Model
#include <linux/kernel.h>         // Contains types, macros, functions for the kernel
#include <linux/fs.h>             // Header for the Linux file system support
#include <linux/uaccess.h>          // Required for the copy to user function

#include "ssd1306.h"

#define  DEVICE_NAME "oled"       ///< The device will appear at /dev/ebbchar using this value
#define  CLASS_NAME  "ebb"        ///< The device class -- this is a character device driver
#define  MAX_MESSAGE 254

static int    majorNumber;                  ///< Stores the device number -- determined automatically
static char   message[MAX_MESSAGE+1] = {0}; ///< Memory for the string that is passed from userspace
static short  size_of_message = 0;          ///< Used to remember the size of the string stored
static struct class*  ebbcharClass  = NULL; ///< The device-driver class struct pointer
static struct device* ebbcharDevice = NULL; ///< The device-driver device struct pointer

static int     dev_open(struct inode *, struct file *);
static int     dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);

static struct file_operations fops =
{
   .owner = THIS_MODULE,
   .open = dev_open,
   .read = dev_read,
   .write = dev_write,
   .release = dev_release,
};

int init_chardev(void){
   // Try to dynamically allocate a major number for the device -- more difficult but worth it
   majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
   if (majorNumber<0){
      printk(KERN_ALERT "Failed to register a major number\n");
      return majorNumber;
   }

   // Register the device class
   ebbcharClass = class_create(THIS_MODULE, CLASS_NAME);
   if (IS_ERR(ebbcharClass)){                // Check for error and clean up if there is
      unregister_chrdev(majorNumber, DEVICE_NAME);
      printk(KERN_ALERT "Failed to register device class\n");
      return PTR_ERR(ebbcharClass);          // Correct way to return an error on a pointer
   }

   // Register the device driver
   ebbcharDevice = device_create(ebbcharClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
   if (IS_ERR(ebbcharDevice)){               // Clean up if there is an error
      class_destroy(ebbcharClass);           // Repeated code but the alternative is goto statements
      unregister_chrdev(majorNumber, DEVICE_NAME);
      printk(KERN_ALERT "Failed to create the device\n");
      return PTR_ERR(ebbcharDevice);
   }
   return 0;
}

void cleanup_chardev(void){
   device_destroy(ebbcharClass, MKDEV(majorNumber, 0));     // remove the device
   //class_unregister(ebbcharClass);                          // unregister the device class
   class_destroy(ebbcharClass);                             // remove the device class
   unregister_chrdev(majorNumber, DEVICE_NAME);             // unregister the major number
}

static int dev_open(struct inode *inodep, struct file *filep){
   size_of_message = 0;
   message[0] = 0;
   return 0;
}

static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset){
   int error_count = 0;
   if(size_of_message<=0) {
    return 0;
   }
   // copy_to_user has the format ( * to, *from, size) and returns 0 on success
   error_count = copy_to_user(buffer, message, size_of_message);

   if (error_count==0){            // if true then have success
      printk(KERN_INFO "EBBChar: Sent %d characters to the user\n", size_of_message);
      return (size_of_message=0);  // clear the position to the start and return 0
   }
   else {
      printk(KERN_INFO "EBBChar: Failed to send %d characters to the user\n", error_count);
      return -EFAULT;              // Failed -- return a bad address message (i.e. -14)
   }
}

static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset){
   size_t wlen = (size_of_message + len) > MAX_MESSAGE ? MAX_MESSAGE - size_of_message : len;
   int rlen;
   if(len <= 0|| wlen <= 0)
     return 0;
   rlen = copy_from_user(&message[size_of_message], buffer, wlen);
   size_of_message += (wlen - rlen); // store the length of the stored message
   //printk(KERN_INFO "Received %zd characters from the user (%.*s)\n", wlen - rlen, size_of_message, message);
   return wlen;
}

static int dev_release(struct inode *inodep, struct file *filep){
   //printk(KERN_INFO "Writing '%.*s' to OLED\n", size_of_message, message);
   set_string_ssd1306(message, size_of_message);
   return 0;
}

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

#include<linux/fs.h>

#define MAX_LEN 32
#define DEV_NAME "pchar"

static char device_buf[MAX_LEN];
static dev_t devno;

int __init pchar_init(void)
{
    int ret, major, minor;

    printk("******* Initilization of Kernel called - [%s] \n",THIS_MODULE->name);
    
    ret = alloc_chrdev_region(&devno, 0, 1, DEV_NAME);
    if(ret<0)
    {
        printk("!!!!! Alloc_char_dev failed");
    }   
    major = MAJOR(devno);
    minor = MINOR(devno);

    printk("******* Allocated major device number  = %d \n Minor number = %d", major, minor);
    return 0;
}

void __exit pchar_exit(void)
{
    printk("******* Cleaunup module of Kernel called. [%s]\n", THIS_MODULE->name);
    unregister_chrdev_region(devno, 1);
    printk("******* device region unregistered \n", THIS_MODULE->name);
}

module_init(pchar_init);
module_exit(pchar_exit);

MODULE_INFO(license,"GPL");
MODULE_INFO(author,"zodgevaibhav@gmail.com");
MODULE_INFO(description,"Pseudo Character device driver");

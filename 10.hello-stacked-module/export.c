#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>

int my_number = 3;

void my_function(void)
{
    printk("[%s]: my_function() called, my_number = %d.\n", THIS_MODULE->name, my_number);
}

static int __init hello_init(void)
{
    printk("[%s]: init_module() called.\n", THIS_MODULE->name);
    return 0;
}

static void __exit hello_exit(void)
{
    printk("[%s]: cleanup_module() called.\n", THIS_MODULE->name);    
}

EXPORT_SYMBOL(my_function); //Export function, This will be used by another module import.ko
EXPORT_SYMBOL(my_number);   // Export my_number variable, This will be used by another module import.ko

module_init(hello_init);
module_exit(hello_exit);

MODULE_INFO(license, "GPL");
MODULE_INFO(author, "Nilesh Ghule <nilesh@sunbeaminfo.com>");
MODULE_INFO(description, "Module exporting symbols");
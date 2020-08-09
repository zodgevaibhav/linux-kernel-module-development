#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include "export.h"

static int __init hello_init(void)
{
    printk("[%s]: init_module() called: my_number = %d\n", THIS_MODULE->name, my_number); //my numbers is coming from export.ko module
    my_number++;
    my_function();
    return 0;
}

static void __exit hello_exit(void)
{
    my_number++;
    my_function();
    printk("[%s]: cleanup_module() called.\n", THIS_MODULE->name);    
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_INFO(license, "GPL");
MODULE_INFO(author, "Nilesh Ghule <nilesh@sunbeaminfo.com>");
MODULE_INFO(description, "Module importing symbols");
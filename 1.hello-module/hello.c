#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

int init_module(void)
{
    printk("******* Initilization of Kernel called. \n");
    return 0;
}

void cleanup_module(void)
{
    printk("******* Cleaunup module of Kernel called. \n");
}

//module_init(vz_init);
//module_exit(vz_clean);

MODULE_INFO(license,"GPL");
MODULE_INFO(author,"zodgevaibhav@gmail.com");
MODULE_INFO(description,"Basic kernel module");

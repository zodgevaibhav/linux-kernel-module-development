#include<linux/kernel.h>
#include<linux/module.h>

void myFunction(void)
{
    printk("******* From another file");
}
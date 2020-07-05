#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/slab.h>


static int vz_init(void)  //removed __init
{
    struct module * ptr;
    
    printk("******* Initilization of Kernel called. \n");


    list_for_each_entry(ptr,&THIS_MODULE->list, list)
    {
                printk("[%s] %d, %s. \n", THIS_MODULE->name, ptr->name);
    }

    return 0;
}

static void vz_cleanup(void) //removed __exit
{
    printk("******* Cleaunup module of Kernel called. \n");   
}

module_init(vz_init);
module_exit(vz_cleanup);

MODULE_INFO(license,"GPL");
MODULE_INFO(author,"zodgevaibhav@gmail.com");
MODULE_INFO(description,"Basic kernel module");

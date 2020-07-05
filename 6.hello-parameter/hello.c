#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

static int cnt = 3;
static char *uname = "vzodge";

static int vz_init(void)  //removed __init
{
    int i;
    printk("******* Initilization of Kernel called. \n");
    for(i=0;i<cnt;i++ )
    {
        printk("[%s] Hello, %s. \n",THIS_MODULE->name,uname);
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

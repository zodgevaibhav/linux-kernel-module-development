#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/moduleparam.h>

static int cnt = 3;
static char *uname = "vzodge";
static int numbers[3]={2,6,8};

module_param_named(count,cnt, int, 0644);
module_param(uname, charp, 0644);
module_param_array(numbers, int, numbers, 0644);
MODULE_PARM_DESC(numbers,"Int Array of size 3");

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

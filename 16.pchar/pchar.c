#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

#include<linux/fs.h>
#include <linux/cdev.h>

#define MAX_LEN 32
#define DEV_NAME "pchar"
#define DEV_CLASS_NAME "_pchar_class"

static char device_buf[MAX_LEN];
static dev_t devno;

static struct cdev pchar_cdev;
static struct class *pclass;
static struct device *pdevice;


static int pchar_open(struct inode *pinode, struct file *pfile) ; //Professional linux kernel architecute book, Chapter 8, 8.3.4 File Operations : file_operations (page - 537)
static int pchar_release(struct inode *pinode, struct file *pfile);
static ssize_t pchar_read(struct file *pfile, char __user *buf, size_t len, loff_t *pf_pos); //__user says this buffer coming from user space
static ssize_t pchar_write(struct file *pfile, const char __user *buf, size_t len, loff_t *pf_pos);

static struct file_operations pchar_fops = {  //tagged structure initilization
    .owner = THIS_MODULE,
    .open = pchar_open,
    .release = pchar_release,
    .read = pchar_read,
    .write = pchar_write
};

int __init pchar_init(void)
{
    int ret, major, minor;

    printk("******* Initilization of Kernel called - [%s] \n",THIS_MODULE->name);
    
    // Step 1 : Allocate character device region
        devno = MKDEV(250, 0);
        
        ret = alloc_chrdev_region(&devno, 0, 1, DEV_NAME);
        if(ret<0)
        {
            printk("!!!!! Alloc_char_dev failed");
            goto alloc_chrdev_region_err;
        }   
        major = MAJOR(devno);
        minor = MINOR(devno);

    //Step 2 : Initialize created cdev and add in to kernel DB
        cdev_init(&pchar_cdev, &pchar_fops);


        ret = cdev_add(&pchar_cdev,devno, 1);

        if(ret<0)
        {
            printk("!!!!! cdev_add failed");
            goto cdev_add_err;
        }

        printk("******* cdev_add success");
        printk("******* Allocated major device number  = %d \n Minor number = %d", major, minor);

    //Step 3 : Create device class
        pclass = class_create(THIS_MODULE,DEV_CLASS_NAME );
        if(pclass==NULL)
        {
            printk("!!!!! class creation failed");
            goto class_create_err;
        }

    //Step 4 : Create devide file
        pdevice = device_create(pclass, NULL, devno, NULL, DEV_CLASS_NAME);
        if(IS_ERR(pdevice))
        {
            printk("!!!!! Device create fail");
            goto class_create_err;
        }
        return 0;

class_create_err:
    class_destroy(pclass);
    printk("!!!!!! Class destroyed \n", THIS_MODULE->name);
cdev_add_err:
    unregister_chrdev_region(devno, 1);
    printk("!!!!!!! device region unregistered \n", THIS_MODULE->name);
alloc_chrdev_region_err:
    printk("!!!!!! CDEV add failed \n", THIS_MODULE->name);
    return ret;
}

void __exit pchar_exit(void)
{
    printk("******* Cleaunup module of Kernel called. [%s]\n", THIS_MODULE->name);
    class_destroy(pclass);
    
    cdev_del(&pchar_cdev);
    printk("******* cdev release done \n", THIS_MODULE->name);

    unregister_chrdev_region(devno, 1);
    printk("******* device region unregistered \n", THIS_MODULE->name);
}



 static int pchar_open(struct inode *pinode, struct file *pfile) //Professional linux kernel architecute book, Chapter 8, 8.3.4 File Operations : file_operations (page - 537)
{
    printk("******* pchar_open\n", THIS_MODULE->name);
    return 0;
}
 static int pchar_release(struct inode *pinode, struct file *pfile)
{
    printk("******* pchar_release\n", THIS_MODULE->name);
    return 0;
}
 static ssize_t pchar_read(struct file *pfile, char __user *buf, size_t len, loff_t *pf_pos) //__user says this buffer coming from user space
{
    printk("******* pchar_read\n", THIS_MODULE->name);
    return len;
}

 static ssize_t pchar_write(struct file *pfile, const char __user *buf, size_t len, loff_t *pf_pos)
 {
     printk("******* pchar_write\n", THIS_MODULE->name);
     return len;
 }

module_init(pchar_init);
module_exit(pchar_exit);

MODULE_INFO(license,"GPL");
MODULE_INFO(author,"zodgevaibhav@gmail.com");
MODULE_INFO(description,"Pseudo Character device driver");

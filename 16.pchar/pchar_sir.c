#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/cdev.h>

#define DEV_NAME "pchar"
#define MAXLEN 32

static char dev_buf[MAXLEN];
static dev_t devno;
static struct cdev pchar_cdev;

static int pchar_open(struct inode *pinode, struct file *pfile);
static int pchar_release(struct inode *pinode, struct file *pfile);
static ssize_t pchar_read(struct file *pfile, char __user *buf, size_t len, loff_t *pf_pos);
static ssize_t pchar_write(struct file *pfile, const char __user *buf, size_t len, loff_t *pf_pos);

static struct file_operations pchar_fops = {
    .owner = THIS_MODULE,
    .open = pchar_open,
    .release = pchar_release,
    .read = pchar_read,
    .write = pchar_write
};

static int __init pchar_init(void)
{
    int ret, major, minor;
    printk("[%s]: pchar_init() called.\n", THIS_MODULE->name);
    // allocate device number
    devno = MKDEV(250, 0);
    ret = alloc_chrdev_region(&devno, 0, 1, DEV_NAME);
    if(ret < 0) {
        printk("[%s]: alloc_chrdev_region() failed.\n", THIS_MODULE->name);
        goto alloc_chrdev_region_err;
    }
    major = MAJOR(devno);
    minor = MINOR(devno);
    printk("[%s]: alloc_chrdev_region() allocated devno = %d / %d.\n", THIS_MODULE->name, major, minor);
    
    // init cdev and add into the kernel db
    cdev_init(&pchar_cdev, &pchar_fops);
    ret = cdev_add(&pchar_cdev, devno, 1);
    if(ret < 0) {
        printk("[%s]: cdev_add() failed.\n", THIS_MODULE->name);
        goto cdev_add_err;
    }
    printk("[%s]: cdev_add() successful.\n", THIS_MODULE->name);
    
    return 0;

cdev_add_err:
    unregister_chrdev_region(devno, 1);
    printk("[%s]: unregister_chrdev_region() released devno.\n", THIS_MODULE->name);
alloc_chrdev_region_err:    
    return ret;
}

static void __exit pchar_exit(void)
{
    printk("[%s]: pchar_exit() called.\n", THIS_MODULE->name);    
    // delete cdev from the system
    cdev_del(&pchar_cdev);
    printk("[%s]: cdev_del() released cdev.\n", THIS_MODULE->name);
    // de-allocate device number
    unregister_chrdev_region(devno, 1);
    printk("[%s]: unregister_chrdev_region() released devno.\n", THIS_MODULE->name);
}

static int pchar_open(struct inode *pinode, struct file *pfile) {
    return 0;
}

static int pchar_release(struct inode *pinode, struct file *pfile) {
    return 0;
}

static ssize_t pchar_read(struct file *pfile, char __user *buf, size_t len, loff_t *pf_pos) {
    return len;
}

static ssize_t pchar_write(struct file *pfile, const char __user *buf, size_t len, loff_t *pf_pos) {
    return len;
}

module_init(pchar_init);
module_exit(pchar_exit);

MODULE_INFO(license, "GPL");
MODULE_INFO(author, "Nilesh Ghule <nilesh@sunbeaminfo.com>");
MODULE_INFO(description, "Over Simplified Pseudo Char Device Driver LCDD-01 batch");

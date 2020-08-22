#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

#include<linux/fs.h>
#include <linux/cdev.h>

#include<asm/uaccess.h>

#define MAXLEN 32
#define DEV_NAME "vz_pchar"
#define DEV_CLASS_NAME "vz_class"

static char dev_buf[MAXLEN];
static dev_t devno;

static struct cdev pchar_cdev; //
static struct class *pclass;



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
    struct device *pdevice;

    printk("******* Initilization of Kernel called - [%s] \n",THIS_MODULE->name);
    
    // Step 1 : Allocate character device region
        devno = MKDEV(250, 0);
        
        ret = alloc_chrdev_region(&devno, 0, 1, DEV_NAME); // allocate device. It's entry added in /proc/devices (cat /proc/devices)
        if(ret<0)
        {
            printk("!!!!! Alloc_char_dev failed");
            goto alloc_chrdev_region_err;
        }   
        major = MAJOR(devno);  //get major number -> It represent type of device
        minor = MINOR(devno); // get minor number -> It represemt device number (hard ware number)

    //Step 2 : Initialize created cdev 
        cdev_init(&pchar_cdev, &pchar_fops); //file_operation mapped with cdev

    // Step 3 : add in to kernel DB
        ret = cdev_add(&pchar_cdev,devno, 1); //Device represented with struct_cdev (it is kernel Object)

        if(ret<0)
        {
            printk("!!!!! cdev_add failed");
            goto cdev_add_err;
        }

        printk("******* cdev_add success");
        printk("******* Allocated major device number  = %d \n Minor number = %d", major, minor);

    //Step 4 : Create device class /sys/class/vz_class   //Create Device File Step-1 
        pclass = class_create(THIS_MODULE,DEV_CLASS_NAME );
        if(pclass==NULL)
        {
            printk("!!!!! class creation failed");
            goto class_create_err;
        }

    //Step 5 : Create devide file in /device/pchar //Create Device File Step-2
        pdevice = device_create(pclass, NULL, devno, NULL, DEV_NAME);
        if(IS_ERR(pdevice))
        {
            printk("!!!!! Device create fail");
            goto dev_create_err;
        }
        return 0;

dev_create_err:
    class_destroy(pclass);
    printk("!!!!!! Class destroyed \n", THIS_MODULE->name);

class_create_err:
    cdev_del(&pchar_cdev);
    printk("!!!!!! dev destroyed \n", THIS_MODULE->name);
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

    device_destroy(pclass, devno);

    class_destroy(pclass);
    
    cdev_del(&pchar_cdev);
    printk("******* cdev release done \n", THIS_MODULE->name);

    unregister_chrdev_region(devno, 1);
    printk("******* device region unregistered \n", THIS_MODULE->name);
}



 static int pchar_open(struct inode *pinode, struct file *pfile) //Professional linux kernel architecute book, Chapter 8, 8.3.4 File Operations : file_operations (page - 537)
{
    printk("******* pchar_open\n");
    return 0;
}
 static int pchar_release(struct inode *pinode, struct file *pfile)
{
    printk("******* pchar_release\n");
    return 0;
}
 static ssize_t pchar_read(struct file *pfile, char __user *buf, size_t len, loff_t *pf_pos) //__user says this buffer coming from user space
{
    int avail, bytes_to_read, nbytes;
	avail = MAXLEN - *pf_pos;
    bytes_to_read = len < avail ? len : avail;
	if(bytes_to_read <= 0) {
		printk("[%s]: pchar_read() reached to end of device.\n", THIS_MODULE->name);
		return 0;
	}
	nbytes = bytes_to_read - raw_copy_to_user(buf, dev_buf + *pf_pos, bytes_to_read);
	*pf_pos = *pf_pos + nbytes;
	printk("[%s]: pchar_read() read %d bytes from the device.\n", THIS_MODULE->name, nbytes);
	return nbytes;
}

//pf_pos -> pointer to file position
 static ssize_t pchar_write(struct file *pfile, const char __user *buf, size_t len, loff_t *pf_pos)
 {
      int avail, bytes_to_write, nbytes;
	avail = MAXLEN - *pf_pos;
    bytes_to_write = len < avail ? len : avail;
	if(bytes_to_write <= 0) {
		printk("[%s]: pchar_write() no space in device to write.\n", THIS_MODULE->name);
		return -ENOSPC;
	}
	nbytes = bytes_to_write - raw_copy_from_user(dev_buf + *pf_pos, buf, bytes_to_write);
	*pf_pos = *pf_pos + nbytes;
	printk("[%s]: pchar_write() written %d bytes into the device.\n", THIS_MODULE->name, nbytes);
	return nbytes;
 }

module_init(pchar_init);
module_exit(pchar_exit);

MODULE_INFO(license,"GPL");
MODULE_INFO(author,"zodgevaibhav@gmail.com");
MODULE_INFO(description,"Pseudo Character device driver");

/*
############################## OutPut #############################
Command
    sudo insmod pchar.ko  //Insert module
    lsmod | grep pchar    //check if module inserted
    dmesg|tail             //redlog 
    ls /sys/class/vz_class //check device class is crated
    ls /dev/vz_pchar        // check if device is added in kernel object
    echo "Hello" | sudo tee /dev/vz_pchar   //write data to our device driver
    dmesg|tail             //check if character written on driver
    sudo cat /dev/vz_pchar  // read data from our device driver
    dmesg|tail             //check if we can read data from device driver kernel messages

[14046.941620] ******* cdev_add success
[14046.941621] ******* Allocated major device number  = 239 
                Minor number = 0
[14109.187515] ******* pchar_open
[14109.187549] [pchar]: pchar_write() written 6 bytes into the device.
[14109.187558] ******* pchar_release
[14152.387405] ******* pchar_open
[14152.387435] [pchar]: pchar_read() read 32 bytes from the device.
[14152.387457] [pchar]: pchar_read() reached to end of device.
[14152.387478] ******* pchar_release



/*

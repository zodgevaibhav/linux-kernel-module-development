#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>
#include <linux/kfifo.h>
#include <linux/slab.h>
#include <linux/moduleparam.h>

#define DEV_NAME "pchar"
#define DEV_CLASS DEV_NAME "_class"

#define MAXLEN 32

struct pchar_priv {
	struct kfifo fifo;
	struct cdev cdev;
};
static struct pchar_priv *devices = NULL;

static int dev_cnt = 3;
module_param(dev_cnt, int, 0644);

static dev_t devno;
static struct class *pclass;

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
    int ret, major, minor, i;
    struct device *pdevice;
	dev_t temp;

    printk("[%s]: pchar_init() called.\n", THIS_MODULE->name);
	// allocate memory for device private struct -- dev_cnt
	devices = (struct pchar_priv*) kmalloc(dev_cnt * sizeof(struct pchar_priv), GFP_KERNEL);
	if(devices == NULL) {
		printk("[%s]: kmalloc() failed to allocate device priv struct.\n", THIS_MODULE->name);
		ret = -ENOMEM;
		goto kmalloc_err;
	}
	printk("[%s]: kmalloc() succeed to allocate device priv struct for %d devices.\n", THIS_MODULE->name, dev_cnt);

    // allocate device number
    devno = MKDEV(250, 0);
    ret = alloc_chrdev_region(&devno, 0, dev_cnt, DEV_NAME);
    if(ret < 0) {
        printk("[%s]: alloc_chrdev_region() failed.\n", THIS_MODULE->name);
        goto alloc_chrdev_region_err;
    }
    major = MAJOR(devno);
    minor = MINOR(devno);
    printk("[%s]: alloc_chrdev_region() allocated devno = %d / %d.\n", THIS_MODULE->name, major, minor);
    
    // init cdev and add into the kernel db
	for(i=0; i<dev_cnt; i++) {
		cdev_init(&devices[i].cdev, &pchar_fops);
		temp = MKDEV(major, i);
		ret = cdev_add(&devices[i].cdev, temp, 1);
		if(ret < 0) {
			printk("[%s]: cdev_add() failed to add cdev for device %d.\n", THIS_MODULE->name, i);
			goto cdev_add_err;
		}
		printk("[%s]: cdev_add() successful to add cdev for device %d.\n", THIS_MODULE->name, i);
	}

    // create device class
    pclass = class_create(THIS_MODULE, DEV_CLASS);
    if(IS_ERR(pclass)) {
        printk("[%s]: class_create() failed.\n", THIS_MODULE->name);
        goto class_create_err;
    }
    printk("[%s]: class_cleate() created device class.\n", THIS_MODULE->name);

    // create device file
	for(i=0; i<dev_cnt; i++) {
		temp = MKDEV(major, i);
		pdevice = device_create(pclass, NULL, temp, NULL, DEV_NAME "%d", i); // "pchar%d"
		if(IS_ERR(pdevice)) {
			printk("[%s]: device_create() failed to create device %d.\n", THIS_MODULE->name, i);
			goto device_create_err;
		}
		printk("[%s]: device_create() created device file for device %d.\n", THIS_MODULE->name, i);
	}

	// allocate fifo for each device
	for(i=0; i<dev_cnt; i++) {
		ret = kfifo_alloc(&devices[i].fifo, MAXLEN, GFP_KERNEL);
		if(ret != 0) {
			printk("[%s]: kfifo_alloc() failed to allocate fifo for device %d.\n", THIS_MODULE->name, i);
			goto kfifo_alloc_err;
		}
		printk("[%s]: kfifo_alloc() succeeded to allocate fifo for device %d.\n", THIS_MODULE->name, i);
	}
    return 0;

kfifo_alloc_err:
	for(i=i-1; i>=0; i--) {
		kfifo_free(&devices[i].fifo);
		printk("[%s]: kfifo_free() released fifo for device %d.\n", THIS_MODULE->name, i);
	}
	i = dev_cnt;
device_create_err:
	for(i=i-1; i>=0; i--) {
		temp = MKDEV(major, i);
		device_destroy(pclass, temp);
    	printk("[%s]: device_destroy() destroyed device file for %d.\n", THIS_MODULE->name, i);
	}
    class_destroy(pclass);
    printk("[%s]: class_destroy() released device class.\n", THIS_MODULE->name);
class_create_err:
	i = dev_cnt;
cdev_add_err:
	for(i=i-1; i>=0; i--) {
		cdev_del(&devices[i].cdev);
    	printk("[%s]: cdev_del() released cdev for device %d.\n", THIS_MODULE->name, i);
	}
    unregister_chrdev_region(devno, dev_cnt);
    printk("[%s]: unregister_chrdev_region() released devno.\n", THIS_MODULE->name);
alloc_chrdev_region_err:
	kfree(devices);
	printk("[%s]: kfree() released device priv struct memory.\n", THIS_MODULE->name);
kmalloc_err: 
    return ret;
}

static void __exit pchar_exit(void)
{
	int i, major = MAJOR(devno);
	dev_t temp;
    printk("[%s]: pchar_exit() called.\n", THIS_MODULE->name);    
    // release kfifo for all devices
	for(i=dev_cnt-1; i>=0; i--) {
		kfifo_free(&devices[i].fifo);
		printk("[%s]: kfifo_free() released fifo for device %d.\n", THIS_MODULE->name, i);
	}
	// release device file
	for(i=dev_cnt-1; i>=0; i--) {
		temp = MKDEV(major, i);
		device_destroy(pclass, temp);
    	printk("[%s]: device_destroy() destroyed device file for %d.\n", THIS_MODULE->name, i);
	}
    // release device class
    class_destroy(pclass);
    printk("[%s]: class_destroy() released device class.\n", THIS_MODULE->name);
    // delete cdev from the system
	for(i=dev_cnt-1; i>=0; i--) {
		cdev_del(&devices[i].cdev);
    	printk("[%s]: cdev_del() released cdev for device %d.\n", THIS_MODULE->name, i);
	}
    // de-allocate device number
    unregister_chrdev_region(devno, dev_cnt);
    printk("[%s]: unregister_chrdev_region() released devno.\n", THIS_MODULE->name);
	// release device priv struct memory
	kfree(devices);
	printk("[%s]: kfree() released device priv struct memory.\n", THIS_MODULE->name);
}

static int pchar_open(struct inode *pinode, struct file *pfile) {
    printk("[%s]: pchar_open() called.\n", THIS_MODULE->name);
	pfile->private_data = container_of(pinode->i_cdev, struct pchar_priv, cdev);
    return 0;
}

static int pchar_release(struct inode *pinode, struct file *pfile) {
    printk("[%s]: pchar_release() called.\n", THIS_MODULE->name);
    return 0;
}

static ssize_t pchar_read(struct file *pfile, char __user *buf, size_t len, loff_t *pf_pos) {
    struct pchar_priv *dev = (struct pchar_priv *)pfile->private_data;
	int avail, bytes_to_read, nbytes, ret;
	avail = kfifo_len(&dev->fifo);
    bytes_to_read = len < avail ? len : avail;
	if(bytes_to_read <= 0) {
		printk("[%s]: pchar_read() reached to end of device.\n", THIS_MODULE->name);
		return 0;
	}
	ret = kfifo_to_user(&dev->fifo, buf, bytes_to_read, &nbytes);
		// kfifo_out() + copy_to_user() 
	if(ret != 0) {
		printk("[%s]: kfifo_to_user() failed to access user buffer.\n", THIS_MODULE->name);
		return ret;
	}
	printk("[%s]: pchar_read() read %d bytes from the fifo.\n", THIS_MODULE->name, nbytes);
	return nbytes;
}

static ssize_t pchar_write(struct file *pfile, const char __user *buf, size_t len, loff_t *pf_pos) {
    struct pchar_priv *dev = (struct pchar_priv *)pfile->private_data;
    int avail, bytes_to_write, nbytes, ret;
	avail = kfifo_avail(&dev->fifo);
    bytes_to_write = len < avail ? len : avail;
	if(bytes_to_write <= 0) {
		printk("[%s]: pchar_write() no space in fifo to write.\n", THIS_MODULE->name);
		return -ENOSPC;
	}
	ret = kfifo_from_user(&dev->fifo, buf, bytes_to_write, &nbytes);
		// copy_from_user() + kfifo_in();
	if(ret != 0) {
		printk("[%s]: kfifo_from_user() failed to access user buffer.\n", THIS_MODULE->name);
		return ret;
	}
	printk("[%s]: pchar_write() written %d bytes into the fifo.\n", THIS_MODULE->name, nbytes);
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
    ls /sys/devices/virtual/pchar_class/
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
*/

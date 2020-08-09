#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init_task.h>
#include <linux/list.h>
#include <linux/slab.h>

static int __init hello_init(void)
{
    struct task_struct *task;
    struct module *ptr;
    printk("[%s]: init_module() called.\n", THIS_MODULE->name);
    list_for_each_entry(ptr, &THIS_MODULE->list, list) 
    {
        printk("[%s]: %s.\n", THIS_MODULE->name, ptr->name);    
    }
    
    list_for_each_entry(task, &init_task.tasks, tasks)
    {
        printk("[%s]: %d - %s.\n", THIS_MODULE->name, task->pid, task->comm);    
    }
    return 0;
}

static void __exit hello_exit(void)
{
    printk("[%s]: cleanup_module() called.\n", THIS_MODULE->name);    
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_INFO(license, "GPL");
MODULE_INFO(author, "Nilesh Ghule <nilesh@sunbeaminfo.com>");
MODULE_INFO(description, "Hello kernel module demo at LKMP-01 batch");

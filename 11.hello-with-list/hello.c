#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/slab.h>

struct book
{
    int id;
    char name[20];
    struct list_head bk_list;
    
};

static struct book head;


static int vz_init(void)  //removed __init
{
    struct book *p = (struct book*) kmalloc(sizeof(struct book),GFP_KERNEL);
    struct list_head *trav;

    printk("******* Initilization of Kernel called. \n");

    INIT_LIST_HEAD(&head.bk_list);


    
    p->id = 1;
    strcpy(p->name,"C++");
    list_add(&p->bk_list, &head.bk_list);

    p = (struct book*) kmalloc(sizeof(struct book),GFP_KERNEL);
    p->id = 2;
    strcpy(p->name,"C");
    list_add(&p->bk_list, &head.bk_list);
    
    trav = head.bk_list.next;
    while(trav!=&head.bk_list)
    {
        p=container_of(trav, struct book, bk_list);
        printk("[%s] %d, %s. \n", THIS_MODULE->name, p->id, p->name);
    }

    list_for_each_entry(p,&head.bk_list, bk_list)
    {
                printk("[%s] %d, %s. \n", THIS_MODULE->name, p->id, p->name);
    }

    return 0;
}

static void vz_cleanup(void) //removed __exit
{
    struct book *p, *temp;
    printk("******* Cleaunup module of Kernel called. \n");
}

module_init(vz_init);
module_exit(vz_cleanup);

MODULE_INFO(license,"GPL");
MODULE_INFO(author,"zodgevaibhav@gmail.com");
MODULE_INFO(description,"Basic kernel module");

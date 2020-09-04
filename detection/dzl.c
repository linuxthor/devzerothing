#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/uio.h>
#include <linux/slab.h>
#include <linux/kallsyms.h>

unsigned long *zero_foperations;

int init_module(void)
{
    int x;

    // /dev/zero is setup via the zero_fops file_operations structure
    // containing pointers to functions for different operations 
    // (e.g read/write/seek etc)
    zero_foperations = (void *)kallsyms_lookup_name("zero_fops");

    for(x = 0; x < (sizeof(struct file_operations) / sizeof(unsigned long)); x++)
    {
        // check for pointer to suspect address range
        if(*(zero_foperations + x) > MODULES_VADDR)
        {
            printk("zero_fops hmmm\n");
        }
    }
    return 0;
}

void cleanup_module(void)
{      
}

MODULE_AUTHOR("linuxthor");
MODULE_LICENSE("GPL");

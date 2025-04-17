#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>

#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/slab.h>

static struct proc_dir_entry *test_driver_proc = NULL;

static ssize_t test_driver_read(struct file *file, char __user *user, size_t size, loff_t *off)
{
	copy_to_user(user, "Hello!\n", 7);
	return 7;
}

static ssize_t test_driver_write(struct file *file, const char __user *user, size_t size, loff_t *off)
{
	char buff[1024];
	if(size > 1024)
	{
		size = 1024;
	}

	memset(buff, 0x00, 1024);
	copy_from_user(buff, user, size);

	printk("You said: %s\n", buff); 

	return size;
}

static const struct proc_ops test_driver_proc_fops = 
{
	.proc_read = test_driver_read,
	.proc_write = test_driver_write,
};


static int __init test_kernel_driver_init(void)
{
	printk("Test driver says hi!\n");
	test_driver_proc = proc_create("test-driver", 0666, NULL, &test_driver_proc_fops);
	if(test_driver_proc == NULL)
	{
		return -1;
	}	
	return 0;
}

static void __exit test_kernel_driver_exit(void)
{
	printk("Test driver is exiting :( \n");
	proc_remove(test_driver_proc);
	return;
}

module_init(test_kernel_driver_init);
module_exit(test_kernel_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Hunter Wilcox <hunter.wilcox@judgeglass.net>");
MODULE_DESCRIPTION("Test Driver that prints to kernel");
// MODULE_VERISON("1.0");

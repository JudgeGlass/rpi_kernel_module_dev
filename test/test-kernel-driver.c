#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>

static int __init test_kernel_driver_init(void)
{
  printk("Test driver says hi!\n");
  return 0;
}

static void __exit test_kernel_driver_exit(void)
{
  printk("Test driver is exiting :( \n");
  return;
}

module_init(test_kernel_driver_init);
module_exit(test_kernel_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Hunter Wilcox <hunter.wilcox@judgeglass.net>");
MODULE_DESCRIPTION("Test Driver that prints to kernel");
// MODULE_VERISON("1.0");

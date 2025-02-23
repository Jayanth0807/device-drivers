#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/module.h>

MODULE_AUTHOR("jay");
MODULE_LICENSE("GPL");

static int __init hello_init(void)
{
	printk(KERN_ALERT "Hello from the kernel side.!\n");
	return 0;
}

static void __exit hello_exit(void)
{
	printk(KERN_ALERT "Goodbye from Kernel side.!!\n");
	
}


module_init(hello_init);
module_exit(hello_exit);

MODULE_DESCRIPTION("Hello from Kernel");

#include <linux/module.h>
#include <linux/init.h>

/* License, Author, Description, Version .etc declared*/
MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Jingbin Song");
MODULE_DESCRIPTION("Hello World DEMO");
MODULE_VERSION("0.0.1");
MODULE_ALIAS("Chapter 17, Example 1");

/* Init the module */
static int helloworld_init(void)
{
	printk(KERN_ALERT "Hello world module init\n");
	
	return 0;
}

/* clear the module */
static void helloworld_exit(void)
{
	printk(KERN_ALERT "Hellow world module exit\n");
}

module_init(helloworld_init);
module_exit(helloworld_exit);

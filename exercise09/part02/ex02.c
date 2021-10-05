#include <linux/module.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("xone");
MODULE_VERSION("0.01");
MODULE_DESCRIPTION("A simple example Linux module.");

static char retpref[] = "this string returned from ";

char *test_01(void)
{
	static char res[80];
	strcpy(res, retpref);
	strcat(res, __FUNCTION__);
	return res;
}
EXPORT_SYMBOL(test_01);

char *test_02(void)
{
	static char res[80];
	strcpy(res, retpref);
	strcat(res, __FUNCTION__);
	return res;
}
EXPORT_SYMBOL(test_02);

static int __init xinit(void)
{
	printk(KERN_INFO "ex02: Hello!!!\n");
	printk(KERN_INFO "ex02: %s\n", test_01());
	printk(KERN_INFO "ex02: %s\n", test_02());
	return 0;
}

static void __exit xexit(void)
{
	printk(KERN_INFO "ex02: Bye...\n");
}

module_init(xinit);
module_exit(xexit);

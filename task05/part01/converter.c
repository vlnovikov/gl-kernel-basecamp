#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("vnovikov");
MODULE_DESCRIPTION("A simple example Linux module.");
MODULE_VERSION("0.01");

int exchange_rate = 31;

static void uah_to_eu(int amount, int rate, int *l, int *r)
{
	int n = 100 * amount / rate;
	*l = n / 100;
	*r = n % 100;
}

static void eu_to_uah(int amount, int rate, int *l, int *r)
{
	int n = 100 * amount * rate;
	*l = n / 100;
	*r = n % 100;
}

void set_exchange_rate(int rate)
{
	exchange_rate = rate;
}
EXPORT_SYMBOL(set_exchange_rate);

static int __init ex01_init(void)
{
	int uah = 1000;
	int eu = 100;
	int eu_l, eu_r;
	int uah_l, uah_r;
	uah_to_eu(uah, exchange_rate, &eu_l, &eu_r);
	printk(KERN_INFO "Converted %d uah to %d.%d eu", uah, eu_l, eu_r);
	eu_to_uah(eu, exchange_rate, &uah_l, &uah_r);
	printk(KERN_INFO "Converted %d eu to %d.%d uah", eu, uah_l, uah_r);
	return 0;
}

static void __exit ex01_exit(void)
{
	printk(KERN_INFO "Bye...\n");
}

module_init(ex01_init);
module_exit(ex01_exit);

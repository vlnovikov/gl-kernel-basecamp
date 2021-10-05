#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("xone");
MODULE_DESCRIPTION("A simple example Linux module.");
MODULE_VERSION("0.01");

void measure(size_t size, void *(*func)(size_t, gfp_t))
{
	char *p;
	u64 sa, ea, sf, ef;
	sa = ktime_get_ns();
	p = (char *)func(size, GFP_KERNEL);
	sf = ea = ktime_get_ns();
	kfree(p);
	ef = ktime_get_ns();
	printk(KERN_INFO " size: %zu, alloc: %llu, free: %llu", size, ea - sa,
	       ef - sf);
}

void measure_v(size_t size)
{
	char *p;
	u64 sa, ea, sf, ef;
	sa = ktime_get_ns();
	p = (char *)vmalloc(size);
	sf = ea = ktime_get_ns();
	vfree(p);
	ef = ktime_get_ns();
	printk(KERN_INFO " size: %zu, alloc: %llu, free: %llu", size, ea - sa,
	       ef - sf);
}

static int __init init(void)
{
	printk(KERN_INFO "kmalloc:\n");
	measure(1024, kmalloc);
	measure(1024 * 1024, kmalloc);
	measure(1024 * 1024 * 1024, kmalloc);

	printk(KERN_INFO "kzalloc:\n");
	measure(1024, kzalloc);
	measure(1024 * 1024, kzalloc);
	measure(1024 * 1024 * 1024, kzalloc);

	printk(KERN_INFO "vmalloc:\n");
	measure_v(1024);
	measure_v(1024 * 1024);
	measure_v(1024 * 1024 * 1024);

	return -1;
}

static void __exit quit(void)
{
}

module_init(init);
module_exit(quit);

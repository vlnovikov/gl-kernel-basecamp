#include <linux/module.h>
#include <linux/jiffies.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>
#include <linux/pci.h>
#include <linux/version.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/string_helpers.h>
#include <linux/string.h>

MODULE_LICENSE("GPL");

static u32 last_jiffies = 0;

void seconds_to_hh_mm_ss(u32 s, u32 *hh, u32 *mm, u32 *ss)
{
	*ss = s % 60;
	s /= 60;
	*mm = s % 60;
	s /= 60;
	*hh = s;
}

static ssize_t timing_show(struct class *class, struct class_attribute *attr,
			   char *buf)
{
	u32 hh, mm, ss;
	u32 current_jiffies = jiffies;
	seconds_to_hh_mm_ss(current_jiffies / HZ, &hh, &mm, &ss);
	printk(KERN_INFO "~~ timing_show %u\n", current_jiffies);
	sprintf(buf,
		"Seconds since last read: %u\nCurrent jiffies time: %u:%02u:%02u\n",
		(current_jiffies - last_jiffies) / HZ, hh, mm, ss);
	last_jiffies = current_jiffies;
	return strlen(buf);
}

/* sysfs store() method. Calls the store() method corresponding to the individual sysfs file */
static ssize_t timing_store(struct class *class, struct class_attribute *attr,
			    const char *buf, size_t count)
{
	printk(KERN_INFO "~~ timing_store");
	// sscanf(buf, "%d", &current_timing_tracker);
	// printk(KERN_INFO " Write %ld, set %d\n", (long)count,
	//        current_timing_tracker);
	return count;
}

CLASS_ATTR_RW(timing);

static struct class *timing_tracker;

static int __init init(void)
{
	int err;

	if (IS_ERR(timing_tracker)) {
		printk(KERN_INFO " bad class create\n");
	}

	last_jiffies = jiffies;

	timing_tracker = class_create(THIS_MODULE, "timing_tracker");
	err = class_create_file(timing_tracker, &class_attr_timing);

	return 0;
}

void cleanup(void)
{
	class_remove_file(timing_tracker, &class_attr_timing);
	class_destroy(timing_tracker);

	return;
}

module_init(init);
module_exit(cleanup);

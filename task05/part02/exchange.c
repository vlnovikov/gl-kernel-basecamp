#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>
#include <linux/pci.h>
#include <linux/version.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/string_helpers.h>
#include <linux/string.h>

#define MODULE_TAG "Task05"
#define INFO_PREFIX KERN_INFO MODULE_TAG
#define BUFFER_SIZE 16
#define PROC_DIRECTORY "task05"
#define PROC_UAH_TO_EU "uah_to_eu"
#define PROC_EU_TO_UAH "eu_to_uah"

static char *proc_buffer;

static struct proc_dir_entry *proc_dir;
static struct proc_dir_entry *proc_file_uah;
static struct proc_dir_entry *proc_file_eu;

int current_exchange_rate = 31;

typedef struct currency_t {
	int amount;
	int l;
	int r;
	int read;
	int write;
};

static struct currency_t uah = { .amount = 0,
				 .l = 0,
				 .r = 0,
				 .read = 0,
				 .write = 0 };
static struct currency_t eu = { .amount = 0,
				.l = 0,
				.r = 0,
				.read = 0,
				.write = 0 };

/**
 *
 * Basic functions
 *
 */
void uah_to_eu(void)
{
	int n = 100 * uah.amount / current_exchange_rate;
	eu.l = n / 100;
	eu.r = n % 100;
	printk(INFO_PREFIX " Converted %d uah to %d.%d eu", uah.amount, eu.l,
	       eu.r);
}

void eu_to_uah(void)
{
	int n = 100 * eu.amount * current_exchange_rate;
	uah.l = n / 100;
	uah.r = n % 100;
	printk(INFO_PREFIX " Converted %d eu to %d.%d uah", eu.amount, uah.l,
	       uah.r);
}

/**
 *
 * sysfs functions
 *
 */
/* sysfs show() method. Calls the show() method corresponding to the individual sysfs file */
static ssize_t exchange_show(struct class *class, struct class_attribute *attr,
			     char *buf)
{
	sprintf(buf, "%d\n", current_exchange_rate);
	printk(INFO_PREFIX "read %ld\n", (long)strlen(buf));
	return strlen(buf);
}

/* sysfs store() method. Calls the store() method corresponding to the individual sysfs file */
static ssize_t exchange_store(struct class *class, struct class_attribute *attr,
			      const char *buf, size_t count)
{
	sscanf(buf, "%d", &current_exchange_rate);
	printk(INFO_PREFIX " Write %ld, set %d\n", (long)count,
	       current_exchange_rate);
	return count;
}

CLASS_ATTR_RW(exchange);

static struct class *exchange_rate;

/**
 *
 * procfs functions
 *
 */
static ssize_t uah_to_eu_read(struct file *file_p, char __user *buffer,
			      size_t length, loff_t *offset);
static ssize_t uah_to_eu_write(struct file *file_p, const char __user *buffer,
			       size_t length, loff_t *offset);

static ssize_t eu_to_uah_read(struct file *file_p, char __user *buffer,
			      size_t length, loff_t *offset);
static ssize_t eu_to_uah_write(struct file *file_p, const char __user *buffer,
			       size_t length, loff_t *offset);

static struct proc_ops proc_fops_uah = {
	.proc_read = uah_to_eu_read,
	.proc_write = uah_to_eu_write,
};

static struct proc_ops proc_fops_eu = {
	.proc_read = eu_to_uah_read,
	.proc_write = eu_to_uah_write,
};

static int create_buffer(void)
{
	proc_buffer = kmalloc(BUFFER_SIZE, GFP_KERNEL);
	if (NULL == proc_buffer)
		return -ENOMEM;

	return 0;
}

static void cleanup_buffer(void)
{
	if (proc_buffer) {
		kfree(proc_buffer);
		proc_buffer = NULL;
	}
}

static int create_proc_example(void)
{
	proc_dir = proc_mkdir(PROC_DIRECTORY, NULL);
	if (NULL == proc_dir)
		return -EFAULT;

	proc_file_uah = proc_create(PROC_UAH_TO_EU, S_IFREG | S_IRUGO | S_IWUGO,
				    proc_dir, &proc_fops_uah);
	if (NULL == proc_file_uah)
		return -EFAULT;

	proc_file_eu = proc_create(PROC_EU_TO_UAH, S_IFREG | S_IRUGO | S_IWUGO,
				   proc_dir, &proc_fops_eu);
	if (NULL == proc_file_eu)
		return -EFAULT;

	return 0;
}

static void cleanup_proc_example(void)
{
	if (proc_file_uah) {
		remove_proc_entry(PROC_UAH_TO_EU, proc_dir);
		proc_file_uah = NULL;
	}
	if (proc_file_eu) {
		remove_proc_entry(PROC_EU_TO_UAH, proc_dir);
		proc_file_eu = NULL;
	}
	if (proc_dir) {
		remove_proc_entry(PROC_DIRECTORY, NULL);
		proc_dir = NULL;
	}
}

static ssize_t exchange_read(struct file *file_p, char __user *buffer,
			     size_t length, loff_t *offset,
			     struct currency_t *from, struct currency_t *to)
{
	size_t left;
	if (from->read) {
		from->read = 0;
		return 0;
	}
	from->read = 1;
	sprintf(proc_buffer, "%d.%d\n", to->l, to->r);
	length = strlen(proc_buffer);
	left = copy_to_user(buffer, proc_buffer, length);
	printk(INFO_PREFIX " read %zu bytes", length);
	return length - left;
}

static ssize_t uah_to_eu_read(struct file *file_p, char __user *buffer,
			      size_t length, loff_t *offset)
{
	uah_to_eu();
	return exchange_read(file_p, buffer, length, offset, &uah, &eu);
}

static ssize_t eu_to_uah_read(struct file *file_p, char __user *buffer,
			      size_t length, loff_t *offset)
{
	eu_to_uah();
	return exchange_read(file_p, buffer, length, offset, &eu, &uah);
}

static ssize_t exchange_write(struct file *file_p, const char __user *buffer,
			      size_t length, loff_t *offset,
			      struct currency_t *from)
{
	size_t left;
	if (from->write) {
		from->write = 0;
		return 0;
	}
	from->write = 1;
	left = copy_from_user(proc_buffer, buffer, BUFFER_SIZE);
	sscanf(proc_buffer, "%d", &from->amount);
	printk(INFO_PREFIX " write %zu bytes", length - left);
	return length - left;
}

static ssize_t uah_to_eu_write(struct file *file_p, const char __user *buffer,
			       size_t length, loff_t *offset)
{
	return exchange_write(file_p, buffer, length, offset, &uah);
}

static ssize_t eu_to_uah_write(struct file *file_p, const char __user *buffer,
			       size_t length, loff_t *offset)
{
	return exchange_write(file_p, buffer, length, offset, &eu);
}

static int __init e_init(void)
{
	int err;

	if (IS_ERR(exchange_rate)) {
		printk(INFO_PREFIX " bad class create\n");
	}

	exchange_rate = class_create(THIS_MODULE, "exchange_rate");
	err = class_create_file(exchange_rate, &class_attr_exchange);

	err = create_buffer();
	if (err)
		goto error;

	err = create_proc_example();
	if (err)
		goto error;

	printk(INFO_PREFIX " loaded\n");
	return 0;

error:
	printk(INFO_PREFIX " failed to load\n");
	cleanup_proc_example();
	cleanup_buffer();
	return err;
}

static void __exit e_exit(void)
{
	cleanup_proc_example();
	cleanup_buffer();
	class_remove_file(exchange_rate, &class_attr_exchange);
	class_destroy(exchange_rate);
	printk(INFO_PREFIX " exited\n");
}

module_init(e_init);
module_exit(e_exit);
MODULE_LICENSE("GPL");

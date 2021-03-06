
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <asm/uaccess.h>
#include <linux/slab.h>
#include <linux/string_helpers.h>
#include <linux/string.h>

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Volodymyr Novikov <volodymyr.novikov@globallogic.com>");
MODULE_DESCRIPTION("Module for procfs for reverse order of words in string");
MODULE_VERSION("0.1");

#define MODULE_TAG "exercise10_2"
#define PROC_DIRECTORY "exercise10"
#define PROC_FILENAME "buffer"
#define BUFFER_SIZE 256

static char *proc_buffer;
static size_t proc_msg_length;
static size_t proc_msg_read_pos;

static struct proc_dir_entry *proc_dir;
static struct proc_dir_entry *proc_file;

static ssize_t example_read(struct file *file_p, char __user *buffer,
			    size_t length, loff_t *offset);
static ssize_t example_write(struct file *file_p, const char __user *buffer,
			     size_t length, loff_t *offset);

static struct proc_ops proc_fops = {
	.proc_read = example_read,
	.proc_write = example_write,
};

extern void uah_to_eu(int amount);
extern void eu_to_uah(int amount);

static int create_buffer(void)
{
	proc_buffer = kmalloc(BUFFER_SIZE, GFP_KERNEL);
	if (NULL == proc_buffer)
		return -ENOMEM;
	proc_msg_length = 0;

	return 0;
}

static void cleanup_buffer(void)
{
	if (proc_buffer) {
		kfree(proc_buffer);
		proc_buffer = NULL;
	}
	proc_msg_length = 0;
}

static int create_proc_example(void)
{
	proc_dir = proc_mkdir(PROC_DIRECTORY, NULL);
	if (NULL == proc_dir)
		return -EFAULT;

	proc_file = proc_create(PROC_FILENAME, S_IFREG | S_IRUGO | S_IWUGO,
				proc_dir, &proc_fops);
	if (NULL == proc_file)
		return -EFAULT;

	return 0;
}

static void cleanup_proc_example(void)
{
	if (proc_file) {
		remove_proc_entry(PROC_FILENAME, proc_dir);
		proc_file = NULL;
	}
	if (proc_dir) {
		remove_proc_entry(PROC_DIRECTORY, NULL);
		proc_dir = NULL;
	}
}

static ssize_t example_read(struct file *file_p, char __user *buffer,
			    size_t length, loff_t *offset)
{
	size_t left;

	if (length > (proc_msg_length - proc_msg_read_pos))
		length = (proc_msg_length - proc_msg_read_pos);

	left = copy_to_user(buffer, &proc_buffer[proc_msg_read_pos], length);

	proc_msg_read_pos += length - left;

	if (left)
		printk(KERN_ERR MODULE_TAG
		       " failed to read %zu from %zu chars\n",
		       left, length);
	else
		printk(KERN_NOTICE MODULE_TAG " read %zu chars\n", length);

	return length - left;
}

static ssize_t example_write(struct file *file_p, const char __user *buffer,
			     size_t length, loff_t *offset)
{
	size_t msg_length;
	size_t left;
	int amount;

	if (length > BUFFER_SIZE) {
		printk(KERN_WARNING MODULE_TAG
		       "reduse message length from %zu to %u chars\n",
		       length, BUFFER_SIZE);
		msg_length = BUFFER_SIZE;
	} else
		msg_length = length;

	left = copy_from_user(proc_buffer, buffer, msg_length);

	proc_msg_length = msg_length - left;
	proc_msg_read_pos = 0;

	if (left)
		printk(KERN_ERR MODULE_TAG
		       "failed to write %zu from %zu chars\n",
		       left, msg_length);
	else
		printk(KERN_NOTICE MODULE_TAG "written %zu chars\n",
		       msg_length);

	sscanf(proc_buffer, "%d", &amount);

	eu_to_uah(amount);
	uah_to_eu(amount);

	return length;
}

static int __init example_init(void)
{
	int err;

	err = create_buffer();
	if (err)
		goto error;

	err = create_proc_example();
	if (err)
		goto error;

	printk(KERN_NOTICE MODULE_TAG "loaded\n");
	return 0;

error:
	printk(KERN_ERR MODULE_TAG "failed to load\n");
	cleanup_proc_example();
	cleanup_buffer();
	return err;
}

static void __exit example_exit(void)
{
	cleanup_proc_example();
	cleanup_buffer();
	printk(KERN_NOTICE MODULE_TAG "exited\n");
}

module_init(example_init);
module_exit(example_exit);

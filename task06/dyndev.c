#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/version.h> /* LINUX_VERSION_CODE */
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>

static int major = 0;
module_param(major, int, S_IRUGO);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Volodymyr Novikov");
MODULE_VERSION("1.0");

static ssize_t dev_read(struct file *file, char *buf, size_t count,
			loff_t *ppos);
static ssize_t dev_write(struct file *, const char __user *, size_t, loff_t *);

static ssize_t proc_write_add(struct file *file_p, const char __user *buffer,
			      size_t length, loff_t *offset);

static ssize_t proc_write_reset(struct file *file_p, const char __user *buffer,
				size_t length, loff_t *offset);

static ssize_t proc_read_stat(struct file *file_p, char __user *buffer,
			      size_t length, loff_t *offset);

static struct proc_ops proc_fops_add = {
	.proc_write = proc_write_add,
};

static struct proc_ops proc_fops_reset = {
	.proc_write = proc_write_reset,
};

static struct proc_ops proc_fops_stat = {
	.proc_read = proc_read_stat,
};

static const struct file_operations dev_fops = { .owner = THIS_MODULE,
						 .read = dev_read,
						 .write = dev_write };

#define DEVICE_FIRST 0
#define MAX_USERS 16
#define MAX_USER_NAME 32
#define MODNAME "chat_mod"
#define DEFAULT_BUFFER_SIZE 1024

#define PROC_DIRECTORY "chat"
#define PROC_FILENAME_ADD "add_user"
#define PROC_FILENAME_RESET "reset"
#define PROC_FILENAME_STAT "stat"

static struct cdev hcdev;
static struct class *devclass;

static struct proc_dir_entry *proc_dir;
static struct proc_dir_entry *proc_file_add;
static struct proc_dir_entry *proc_file_reset;
static struct proc_dir_entry *proc_file_stat;

static size_t user_count = 0;
static char users[MAX_USERS][MAX_USER_NAME];

static char *chat_buffer;
static size_t chat_buffer_size = DEFAULT_BUFFER_SIZE;
static size_t bytes_read = 0;

static ssize_t dev_read(struct file *file, char *buf, size_t count,
			loff_t *ppos)
{
	char *in = chat_buffer;
	char *out = buf;
	char *next;
	size_t left = 0;
	unsigned int index = iminor(file->f_inode);
	size_t un_l = strlen(users[index]);
	if (bytes_read) {
		bytes_read = 0;
		return bytes_read;
	}
	while (*in) {
		next = strchr(in, '\n');
		if (next)
			++next;
		if (index == 0 ||
		    (memcmp(in, users[index], un_l) == 0 && in[un_l] == ':')) {
			size_t l = next ? next - in : strlen(in);
			left = copy_to_user(out, in, l);
			out += l;
		}
		in = next;
	}
	bytes_read = out - buf;
	printk(KERN_INFO "=== read : %zu", bytes_read);
	return bytes_read;
}

static ssize_t dev_write(struct file *file, const char __user *buf,
			 size_t count, loff_t *ppos)
{
	const char *in = buf;
	char *out = chat_buffer;
	size_t left = 0;
	unsigned int index = iminor(file->f_inode);
	if (index == 0) {
		return 0;
	}
	out = strcat(out, users[index]);
	out = strcat(out, ": ");
	left = copy_from_user(out + strlen(out), in, count);
	printk(KERN_INFO "=== written: %zu index %u", count - left, index);
	return count - left;
}

static ssize_t proc_write_add(struct file *file_p, const char __user *buffer,
			      size_t length, loff_t *offset)
{
	char *user_name = kmalloc(length, __GFP_ZERO);
	dev_t dev = MKDEV(major, DEVICE_FIRST + user_count);
	size_t left = copy_from_user(user_name, buffer, length);
	user_name[length - 1] = 0;
	device_create(devclass, NULL, dev, NULL, "%s_%s", "chat", user_name);
	strcpy(users[user_count], user_name);
	++user_count;
	printk(KERN_INFO "=== user added : %s %zu\n", user_name,
	       strlen(user_name));
	kfree(user_name);
	return length - left;
}

static ssize_t proc_write_reset(struct file *file_p, const char __user *buffer,
				size_t length, loff_t *offset)
{
	unsigned long s = 0;
	int ret = kstrtoul_from_user(buffer, length, 10, &s);
	if (ret < 0) {
		return 0;
	}
	if (s) {
		chat_buffer_size = s * 1024;
		kfree(chat_buffer);
		chat_buffer = kmalloc(chat_buffer_size, __GFP_ZERO);
	}
	return length;
}

static ssize_t proc_read_stat(struct file *file_p, char __user *buffer,
			      size_t length, loff_t *offset)
{
	static ssize_t sent = 0;
	ssize_t left = 0;
	char buf[128];
	if (sent) {
		sent = 0;
		return sent;
	}
	sprintf(buf, "Buffer size: %zu\nUsed: %zu\nUsers: %zu\n",
		chat_buffer_size, strlen(chat_buffer), user_count - 1);
	sent = strlen(buf);
	left = copy_to_user(buffer, buf, sent);
	return sent - left;
}

static int create_proc_files(void)
{
	proc_dir = proc_mkdir(PROC_DIRECTORY, NULL);
	if (NULL == proc_dir)
		return -EFAULT;

	proc_file_add =
		proc_create(PROC_FILENAME_ADD, S_IFREG | S_IRUGO | S_IWUGO,
			    proc_dir, &proc_fops_add);
	if (NULL == proc_file_add)
		return -EFAULT;

	proc_file_reset =
		proc_create(PROC_FILENAME_RESET, S_IFREG | S_IRUGO | S_IWUGO,
			    proc_dir, &proc_fops_reset);
	if (NULL == proc_file_reset)
		return -EFAULT;

	proc_file_stat =
		proc_create(PROC_FILENAME_STAT, S_IFREG | S_IRUGO | S_IWUGO,
			    proc_dir, &proc_fops_stat);
	if (NULL == proc_file_stat)
		return -EFAULT;

	return 0;
}

static void cleanup_proc(void)
{
	if (proc_file_add) {
		remove_proc_entry(PROC_FILENAME_ADD, proc_dir);
		proc_file_add = NULL;
	}
	if (proc_file_reset) {
		remove_proc_entry(PROC_FILENAME_RESET, proc_dir);
		proc_file_reset = NULL;
	}
	if (proc_file_stat) {
		remove_proc_entry(PROC_FILENAME_STAT, proc_dir);
		proc_file_stat = NULL;
	}
	if (proc_dir) {
		remove_proc_entry(PROC_DIRECTORY, NULL);
		proc_dir = NULL;
	}
}

static char *chat_devnode(struct device *dev, umode_t *mode)
{
	if (!mode)
		return NULL;
	if (dev->devt == MKDEV(major, 0) ||
	    dev->devt == MKDEV(major, MAX_USERS))
		*mode = 0666;
	return NULL;
}

static int __init dev_init(void)
{
	dev_t dev;
	int ret;
	chat_buffer = (char *)kmalloc(chat_buffer_size, __GFP_ZERO);
	ret = create_proc_files();
	if (ret < 0) {
		goto err;
	}
	if (major != 0) {
		dev = MKDEV(major, DEVICE_FIRST);
		ret = register_chrdev_region(dev, MAX_USERS, MODNAME);
	} else {
		ret = alloc_chrdev_region(&dev, DEVICE_FIRST, MAX_USERS,
					  MODNAME);
		major = MAJOR(dev); // не забыть зафиксировать!
	}
	if (ret < 0) {
		printk(KERN_ERR "=== Can not register char device region\n");
		goto err;
	}
	cdev_init(&hcdev, &dev_fops);
	hcdev.owner = THIS_MODULE;
	ret = cdev_add(&hcdev, dev, MAX_USERS);
	if (ret < 0) {
		unregister_chrdev_region(MKDEV(major, DEVICE_FIRST), MAX_USERS);
		printk(KERN_ERR "=== Can not add char device\n");
		goto err;
	}
	devclass = class_create(THIS_MODULE, "dyn_class"); /* struct class* */
	devclass->devnode = chat_devnode;

	device_create(devclass, NULL, dev, NULL, "%s_%s", "chat", "all");
	strcpy(users[user_count], "all");
	++user_count;

	return ret;
err:
	kfree(chat_buffer);
	return ret;
}
module_init(dev_init);

static void __exit dev_exit(void)
{
	dev_t dev;
	size_t i;
	kfree(chat_buffer);
	cleanup_proc();
	for (i = 0; i < user_count; i++) {
		dev = MKDEV(major, DEVICE_FIRST + i);
		device_destroy(devclass, dev);
	}
	class_destroy(devclass);
	cdev_del(&hcdev);
	unregister_chrdev_region(MKDEV(major, DEVICE_FIRST), MAX_USERS);
	printk(KERN_INFO "=============== module removed ==================\n");
}
module_exit(dev_exit);

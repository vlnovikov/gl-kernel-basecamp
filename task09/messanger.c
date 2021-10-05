#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/cdev.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <asm/uaccess.h>

MODULE_LICENSE("GPL");

#define THREAD_COUNT 4
#define MAX_MESSAGE_LENGTH 64

static unsigned int index = 0;

struct message {
	unsigned int delay;
	unsigned int total;
	char text[MAX_MESSAGE_LENGTH];
	struct task_struct *task;
	struct list_head next;
};
LIST_HEAD(messages);

static int func(void *data)
{
	unsigned int i = 0;
	struct message *m = data;
	ktime_t s = ktime_get();
	while (!kthread_should_stop() &&
	       (ktime_get() - s) / 1000000 <= m->total) {
		printk(KERN_INFO "M: %s", m->text);
		++i;
		msleep(m->delay);
	}
	m->task = 0;
	return 0;
}

static ssize_t messanger_show(struct class *class, struct class_attribute *attr,
			      char *buf)
{
	return 0;
}

/* sysfs store() method. Calls the store() method corresponding to the individual sysfs file */
static ssize_t messanger_store(struct class *class,
			       struct class_attribute *attr, const char *buf,
			       size_t count)
{
	struct message *m = kmalloc(sizeof(struct message), GFP_KERNEL);
	list_add(&m->next, &messages);
	if (sscanf(buf, "%d %d", &m->delay, &m->total) != 2) {
		printk(KERN_WARNING " Wrong message data\n");
		return count;
	}
	strncpy(m->text, strchr(strchr(buf, ' ') + 1, ' ') + 1,
		MAX_MESSAGE_LENGTH);
	m->task = kthread_run(func, m, "messanger/%u", index);
	++index;
	return count;
}

CLASS_ATTR_RW(messanger);

static struct class *messanger;

static int __init messanger_init(void)
{
	messanger = class_create(THIS_MODULE, "messanger");
	return class_create_file(messanger, &class_attr_messanger);
}

static void __exit messanger_exit(void)
{
	struct list_head *it;

	list_for_each (it, &messages) {
		struct message *m = list_entry(it, struct message, next);
		if (m->task) {
			kthread_stop(m->task);
		}
		kfree(m);
	}

	class_remove_file(messanger, &class_attr_messanger);
	class_destroy(messanger);
}

module_init(messanger_init);
module_exit(messanger_exit);
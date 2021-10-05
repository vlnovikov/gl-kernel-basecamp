#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>

MODULE_LICENSE("GPL");

static struct hrtimer hr_timer;

#define USER_LENGTH 32
#define MESSAGE_LENGTH 128
#define NS 1000000000

struct reminder {
	ktime_t trigger_time;
	char user[USER_LENGTH];
	char message[MESSAGE_LENGTH];
	struct list_head next;
};
LIST_HEAD(reminders);

char current_user[USER_LENGTH];

void add_message(struct reminder *r)
{
	struct list_head *it;
	list_for_each (it, &reminders) {
		struct reminder *e = list_entry(it, struct reminder, next);
		if (e->trigger_time > r->trigger_time)
			break;
	}
	printk(KERN_INFO
	       "Adding message. User: %s | Message: %s | Timeout: %lld",
	       r->user, r->message, r->trigger_time / NS);
	list_add_tail(&(r->next), it);
}

void print_messages(char *buffer, const size_t buffer_size, const char *user)
{
	struct reminder *r;
	struct list_head *it;
	char *ptr = buffer;
	list_for_each (it, &reminders) {
		r = list_entry(it, struct reminder, next);
		if (user && strcmp(r->user, user)) {
			continue;
		}
		snprintf(ptr, buffer_size, "%s <- %lld : %s\n", r->user,
			 r->trigger_time / NS, r->message);
		ptr = ptr + strlen(ptr);
	}
}

void trigger_messages(void)
{
	struct reminder *r;
	struct list_head *i, *in;
	list_for_each_safe (i, in, &reminders) {
		r = list_entry(i, struct reminder, next);
		if (r->trigger_time <= ktime_get()) {
			printk(KERN_INFO "%s: %s", r->user, r->message);
			list_del(i);
			kfree(r);
		} else {
			break;
		}
	}
}

static ssize_t recorder_show(struct class *class, struct class_attribute *attr,
			     char *buf)
{
	const size_t buffer_size = 2048;
	print_messages(buf, buffer_size, *current_user ? current_user : 0);
	return strlen(buf);
}

static ssize_t recorder_store(struct class *class, struct class_attribute *attr,
			      const char *buf, size_t count)
{
	char u[USER_LENGTH];
	ktime_t t;
	char *m_ptr;
	char *m_last;
	struct reminder *r;
	if (sscanf(buf, "%s %lld", u, &t) != 2) {
		printk(KERN_INFO "Failed to parse %s", buf);
		return count;
	}
	m_ptr = strchr(strchr(buf, ' ') + 1, ' ') + 1;
	m_last = min(strchr(m_ptr, '\n'), strchr(m_ptr, 0));
	if (m_ptr - buf >= count) {
		printk(KERN_INFO "Failed to get message for %s", buf);
		return count;
	}
	r = kmalloc(sizeof(struct reminder), GFP_KERNEL);
	strncpy(r->user, u, USER_LENGTH);
	strncpy(r->message, m_ptr, (size_t)(m_last - m_ptr));
	r->trigger_time = ktime_get() + ktime_set(t, 0);
	add_message(r);
	return count;
}

CLASS_ATTR_RW(recorder);

static struct class *records_writer;

enum hrtimer_restart timer_callback(struct hrtimer *timer_for_restart)
{
	ktime_t currtime, interval;
	currtime = ktime_get();
	interval = ktime_set(1, 0);

	trigger_messages();

	hrtimer_forward(timer_for_restart, currtime, interval);
	return HRTIMER_RESTART;
}

static int __init reminder_init(void)
{
	int err;
	ktime_t ktime = ktime_set(1, 0);

	if (IS_ERR(records_writer)) {
		printk(KERN_INFO " bad class create\n");
	}

	records_writer = class_create(THIS_MODULE, "records_writer");
	err = class_create_file(records_writer, &class_attr_recorder);

	hrtimer_init(&hr_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	hr_timer.function = &timer_callback;
	hrtimer_start(&hr_timer, ktime, HRTIMER_MODE_REL);
	return 0;
}

static void __exit reminder_exit(void)
{
	struct reminder *r;
	struct list_head *i, *i_safe;

	hrtimer_cancel(&hr_timer);

	list_for_each_safe (i, i_safe, &reminders) {
		r = list_entry(i, struct reminder, next);
		printk(KERN_INFO "deleting active: %lld", r->trigger_time / NS);
		list_del(i);
		kfree(r);
	}

	class_remove_file(records_writer, &class_attr_recorder);
	class_destroy(records_writer);
}

module_init(reminder_init);
module_exit(reminder_exit);
/*******************************************************************************
* Copyright (c) 2015 Song Yang @ ittraining
*
* All rights reserved.
* This program is free to use, but the ban on selling behavior.
* Modify the program must keep all the original text description.
*
* Email: onionys@ittraining.com.tw
* Blog : http://blog.ittraining.com.tw
*******************************************************************************/
#include <linux/module.h>
#include <linux/init.h>
#include <linux/timer.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ITtraining.com.tw");
MODULE_DESCRIPTION("A timer example.");

struct timer_list my_timer;

/*
 * TIMER FUNCTION
 * */

static void timer_function(struct timer_list *timer)
{
	printk(KERN_INFO "~~ Time up");
	// modify the timer for next time
	mod_timer(timer, jiffies + HZ / 2);
}

/*
 * INIT MODULE
 * */
static int __init init(void)
{
	printk(KERN_INFO "~~ Hello My Timer\n");

	//  -- initialize the timer
	timer_setup(&my_timer, timer_function, 0);
	my_timer.expires = jiffies + HZ;
	my_timer.function = timer_function;

	// -- TIMER START
	add_timer(&my_timer);
	printk(KERN_INFO "~~ END: init_module() \n");
	return 0;
}
module_init(init);

/*
 * CLEANUP MODULE
 * */
void cleanup(void)
{
	del_timer(&my_timer);
	printk(KERN_INFO "~~ Goodbye\n");
}

module_exit(cleanup);

// skeleton.c
#include <linux/module.h>	// needed by all modules
#include <linux/init.h>		// needed for macros
#include <linux/kernel.h>	// needed for debugging
#include <linux/moduleparam.h>	// needed for module parameters
#include <linux/io.h>
#include <linux/ioport.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/mutex.h>
#include <linux/wait.h>

static struct task_struct* my_thread;
int thread_fn(void* data){
	while(!kthread_should_stop()){
		ssleep(5);
		pr_info("thread heartbeat\n");
	}
	return 0;
}
static int __init skeleton_init(void)
{
	pr_info ("starting module 06");
	my_thread=kthread_run(thread_fn,0,"sleep_thread");
	pr_info ("end module, you can exit");
	return 0;
}

static void __exit skeleton_exit(void)
{
	kthread_stop(my_thread);
	pr_info ("Linux module skeleton unloaded\n");

}	

module_init (skeleton_init);
module_exit (skeleton_exit);

MODULE_AUTHOR ("Corboz Simon");
MODULE_DESCRIPTION ("Module skeleton");
MODULE_LICENSE ("GPL");
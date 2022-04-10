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

static struct task_struct* thread_1;
static struct task_struct* thread_2;
static atomic_t has_to_wake_up;
DECLARE_WAIT_QUEUE_HEAD(queue1);

int thread_fn_1(void* data){
	while(!kthread_should_stop()){
		int status = wait_event_interruptible(queue1,atomic_read(&has_to_wake_up)!=0||kthread_should_stop());
		atomic_dec(&has_to_wake_up);
		pr_info("thread 1 woke up\n");
	}
	return 0;
}
int thread_fn_2(void* data){
	while(!kthread_should_stop()){
		int status = wait_event_timeout(queue1,kthread_should_stop(),5*HZ);
		pr_info("thread 2 woke up\n");
		atomic_set(&has_to_wake_up, 1);
		wake_up(&queue1);
	}
	return 0;
}
static int __init skeleton_init(void)
{
	pr_info ("starting module 06");
	thread_1=kthread_run(thread_fn_1,0,"sleep_thread");
	thread_2=kthread_run(thread_fn_2,0,"activator_thread");
	pr_info ("end module, you can exit");
	return 0;
}

static void __exit skeleton_exit(void)
{
	kthread_stop(thread_1);
	kthread_stop(thread_2);
	pr_info ("Linux module skeleton unloaded\n");

}	

module_init (skeleton_init);
module_exit (skeleton_exit);

MODULE_AUTHOR ("Corboz Simon");
MODULE_DESCRIPTION ("Module skeleton");
MODULE_LICENSE ("GPL");
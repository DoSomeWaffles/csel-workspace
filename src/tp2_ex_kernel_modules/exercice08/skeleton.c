#include <linux/module.h>	// needed by all modules
#include <linux/init.h>		// needed for macros
#include <linux/kernel.h>	// needed for debugging
#include <linux/moduleparam.h>	// needed for module parameters
#include <linux/io.h>
#include <linux/ioport.h>
#include <linux/interrupt.h>
#include <linux/gpio.h>



irqreturn_t k1_interrupt(int irq,void *dev_id){
	pr_info ("K1 PUSHED");
	return IRQ_HANDLED;		
}
irqreturn_t k2_interrupt(int irq,void *dev_id){
	pr_info ("K2 PUSHED");
	return IRQ_HANDLED;		
}
irqreturn_t k3_interrupt(int irq,void *dev_id){
	pr_info ("K3 PUSHED");
	return IRQ_HANDLED;		
}
unsigned int k1_irq;
unsigned int k2_irq;
unsigned int k3_irq;
static int __init skeleton_init(void)
{
	pr_info ("starting module 08");
	int status = gpio_request(0,NULL);
	status = gpio_request(2,NULL);
	status= gpio_request(3,NULL);
 	k1_irq = gpio_to_irq(0);
	k2_irq = gpio_to_irq(2);
	k3_irq = gpio_to_irq(3);
	status = request_irq(k1_irq,k1_interrupt,IRQF_TRIGGER_FALLING,"K1",NULL);
	status = request_irq(k2_irq,k2_interrupt,IRQF_TRIGGER_FALLING,"K2",NULL);
	status = request_irq(k3_irq,k3_interrupt,IRQF_TRIGGER_FALLING,"K3",NULL);
	pr_info ("end module, you can exit");
	return 0;
}

static void __exit skeleton_exit(void)
{	
	free_irq(k1_interrupt,NULL);
	free_irq(k2_interrupt,NULL);
	free_irq(k3_interrupt,NULL);
	gpio_free(0);
	gpio_free(2);
	gpio_free(3);
	pr_info ("Linux module skeleton unloaded\n");

}	

module_init (skeleton_init);
module_exit (skeleton_exit);

MODULE_AUTHOR ("Corboz Simon");
MODULE_DESCRIPTION ("Module skeleton");
MODULE_LICENSE ("GPL");
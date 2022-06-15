// skeleton.c
#include <linux/module.h> // needed by all modules
#include <linux/init.h>	  // needed for macros
#include <linux/kernel.h> // needed for debugging
#include <linux/thermal.h> 
#include <linux/timer.h>

//frequency in nanoseconds
static int[] freqs = {500000000,200000000,100000000,50000000};
static int mode; //1 is manual , 0 is automatic
static int frequency;
static const int LOWER_LIMIT = 35;
static const int UPPER_LIMIT= 45;
static struct class* sysfs_class;
static struct device* sysfs_device;
static struct thermal_zone_device* thermal_zone_dev;
static int current_temperature;

ssize_t sysfs_show_frequency(struct device* dev,
                       struct device_attribute* attr,
                       char* buf)
{
    sprintf(buf, "%d\n", frequency);
    return strlen(buf);
}
ssize_t sysfs_store_frequency(struct device* dev,
                        struct device_attribute* attr,
                        const char* buf,
                        size_t count)
{
    frequency = simple_strtol(buf, 0, 10);
    return count;
}
DEVICE_ATTR(frequency, 0664, sysfs_show_frequency, sysfs_store_frequency);

ssize_t sysfs_show_mode(struct device* dev,
                       struct device_attribute* attr,
                       char* buf)
{
	sprintf(buf, "%d\n", mode);
    return strlen(buf);
}
ssize_t sysfs_store_mode(struct device* dev,
                        struct device_attribute* attr,
                        const char* buf,
                        size_t count)
{
    mode = simple_strtol(buf, 0, 10);
    return count;
}
DEVICE_ATTR(mode, 0664, sysfs_show_mode, sysfs_store_mode);


//2 attributes, mode and frequency
int freq_from_temp(int temp){
	if(temp<LOWER_LIMIT)return freqs[0];
	if(temp<40)return freqs[1];
	if(temp<45)return freqs[2];
	if(tmp>UPPER_LIMIT)return freqs[3]
}

//--------------------IRQ--------------------
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
//--------------------IRQ--------------------
unsigned int k1_irq;
unsigned int k2_irq;
unsigned int k3_irq;

static int __init skeleton_init(void)
{//char based driver
	sysfs_class = class_create(THIS_MODULE, "led_controller_class");
    sysfs_device = device_create(sysfs_class, NULL, skeleton_dev, NULL, "led_controller_device");
    if (status == 0) status = device_create_file(sysfs_device, &dev_attr_mode);
 	if (status == 0) status = device_create_file(sysfs_device, &dev_attr_frequency);
	if (status == 0) thermal_zone_dev =  thermal_zone_get_zone_by_name("cpu-thermal");
	int thermal_zone_get_temp(struct thermal_zone_device*, int* temp);

	if(thermal_zone_dev != NULL) status = thermal_zone_get_temp(thermal_zone_dev,&current_temperature);
	//set base freq
	if(status==0){
		//start timer

		if(mode<1){
			//if we are in automatic mode, immediately put the good frequency
			
		}else{
			//set base freq

		}
		while(1){
		// lire le mode
		// AUTOMATIC MODE
		// needs to check the cpu temp
		// and update the frequency 
		// accordingly
			if(mode<1){
				//get temp
			}else{
				//read the file 
			}
			//modify freq
		}
	}
	
}

static void __exit skeleton_exit(void)
{
	pr_info("Linux module skeleton unloaded\n");
	free_irq(k1_interrupt,NULL);
	free_irq(k2_interrupt,NULL);
	free_irq(k3_interrupt,NULL);
	gpio_free(0);
	gpio_free(2);
	gpio_free(3);
	pr_info ("Linux module skeleton unloaded\n");
}
module_init(skeleton_init);
module_exit(skeleton_exit);
MODULE_AUTHOR("Simon Corboz <simon.corboz@hes-so.ch>");
MODULE_DESCRIPTION("Module skeleton");
MODULE_LICENSE("GPL");
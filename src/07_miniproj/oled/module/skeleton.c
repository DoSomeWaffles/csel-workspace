//BEGIN INCLUDE
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/platform_device.h>
#include <linux/uaccess.h>
#include <linux/module.h> // needed by all modules
#include <linux/init.h>	  // needed for macros
#include <linux/kernel.h> // needed for debugging
#include <linux/thermal.h> 
#include <linux/timer.h>
#include <linux/gpio.h>

//END INCLUDE

//BEGIN MACRO
#define GPIO_LED 10

//END MACRO

//BEGIN ATTRIBUTES DECLARATION
//frequency in nanoseconds
static struct timer_list my_timer;
//simple lookup table 
//enable us to bypass ms_to_hz and hz_to_ms calls in automatic mode
static int FREQ_MS_HZ[4][2]  = {{500,2},
							{200,5},
							{100,10},
							{50,20}};
static unsigned int mode; //>0 is manual , <1 is automatic
static unsigned int frequency;
static const unsigned int LOWER_LIMIT = 35;
static const unsigned int UPPER_LIMIT= 45;
static struct class  *sysfs_class;
static struct device* sysfs_device;
static struct thermal_zone_device* thermal_zone_dev;
static int current_temperature;
static int led_status;
static unsigned int curr_freq;
static int freq_index;
//END ATTRIBUTES DECLARATION

int get_temp(void){
	int status = thermal_zone_get_temp(thermal_zone_dev,&current_temperature);
	return status;
}
int freq_index_from_temp(int temp){
	temp/=1000;
	if(temp<LOWER_LIMIT)return 0;
	if(temp<40)return 1;
	if(temp<45)return 2;
	if(temp>UPPER_LIMIT)return 3;
	return -1;
}
unsigned int hz_to_ms(int hz){
	return 1000/(hz) ;
}
int led_init(void){
	int status;
	status = gpio_request(GPIO_LED,"LED");
	if(status==0){
		status = gpio_direction_output(GPIO_LED,0);
	}
	return status;
}
void led_toggle(void){
	led_status = 1-led_status;
 	gpio_set_value(GPIO_LED,led_status);
}
//SRP 
//this kernel module should ONLY modify timer frequency if needed
//this kernel has no responsability to check if the frequency in MS is right
//the data sanitaziong task is up to the daemon, not the kernel module 
void timer_callback(struct timer_list *timer){
	int status;
	int freq;
	//timer callback;
	//test if mode is auto or manual
	led_toggle();
	if(mode<1){//auto
		get_temp();
		freq_index = freq_index_from_temp(current_temperature);
		freq = FREQ_MS_HZ[freq_index][0];
		//only update the frequency attribute IF NEEDED
		if(curr_freq!=freq){
			curr_freq = freq;
			//modify device attribute to make it readable by daemon
			frequency = FREQ_MS_HZ[freq_index][1];
		}
	}else{//
		unsigned int freq = hz_to_ms(frequency);
		if(freq!=curr_freq){//only update if needed
			curr_freq = freq;
		}
	}
	status = mod_timer(&my_timer,jiffies+msecs_to_jiffies(curr_freq));
}

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



static int __init skeleton_init(void)
{//char based driver
	int status;
	sysfs_class = class_create(THIS_MODULE, "fan_controller_class");
    sysfs_device = device_create(sysfs_class, NULL, 0, NULL, "fan_controller_device0");
    status = device_create_file(sysfs_device, &dev_attr_mode);
 	if (status == 0) status = device_create_file(sysfs_device, &dev_attr_frequency);
	//init mode to automatic
	mode=0;
	led_status  = 0;
	 //---TEMP INIT BEGIN---
	thermal_zone_dev =  thermal_zone_get_zone_by_name("cpu-thermal");
	//---TEMP INIT END---
	
	//---LED INIT BEGIN ---
	if(status==0)status=led_init();
	//---LED INIT END---
	
	//---TIMER INIT BEGIN---
	timer_setup(&my_timer,timer_callback,0);
	if(thermal_zone_dev != NULL)status = thermal_zone_get_temp(thermal_zone_dev,&current_temperature);
	freq_index = freq_index_from_temp(current_temperature);
	curr_freq = FREQ_MS_HZ[0][0];
	frequency = FREQ_MS_HZ[0][1];
	status = mod_timer(&my_timer,jiffies+msecs_to_jiffies(curr_freq));
	//get initial freq from temp so as to have an adequate fan frequency on startup
	//---TIMER INIT END---
	
	return 0;
}

static void __exit skeleton_exit(void)
{
	pr_info("Linux module skeleton unloaded\n");
	del_timer(&my_timer);
	gpio_free(GPIO_LED);
	device_remove_file(sysfs_device,&dev_attr_frequency);
	device_remove_file(sysfs_device,&dev_attr_mode);
	device_destroy(sysfs_class,0);
	class_destroy(sysfs_class);
	pr_info ("Linux module skeleton unloaded\n");
}
module_init(skeleton_init);
module_exit(skeleton_exit);
MODULE_AUTHOR("Simon Corboz <simon.corboz@hes-so.ch>");
MODULE_DESCRIPTION("Module skeleton");
MODULE_LICENSE("GPL");
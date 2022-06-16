//BEGIN INCLUDE
#include <linux/module.h> // needed by all modules
#include <linux/init.h>	  // needed for macros
#include <linux/kernel.h> // needed for debugging
#include <linux/thermal.h> 
#include <linux/timer.h>
#include <linux/gpio.h>
//END INCLUDE

//BEGIN MACRO
#define GPIO_LED 10
#define GET_JIFFIE_FROM_FREQ

//END MACRO

//BEGIN ATTRIBUTES DECLARATION
//frequency in nanoseconds
static struct timer_list my_timer;

static int[][] FREQS_MS_HZ = {{500000,2},
							{200000,5},
							{100000,10},
							{50000,20}};
static unsigned int mode; //>0 is manual , <1 is automatic
static unsigned int frequency;
static const unsigned int LOWER_LIMIT = 35;
static const unsigned int UPPER_LIMIT= 45;
static struct class* sysfs_class;
static struct device* sysfs_device;
static struct thermal_zone_device* thermal_zone_dev;
static int current_temperature;
static int led_status;
//END ATTRIBUTES DECLARATION


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

int get_temp(){
	int status = thermal_zone_get_temp(thermal_zone_dev,&current_temperature);
	return status;
}
int freq_from_temp(int temp){
	temp /=1000;
	if(temp<LOWER_LIMIT)return freqs[0][0];
	if(temp<40)return freqs[1][0];
	if(temp<45)return freqs[2][0];
	if(tmp>UPPER_LIMIT)return freqs[3][0];
}
int hz_to_ms(int hz){

}
int led_init(){
	int status;
	status = gpio_request(GPIO_LED,"LED");
	if(status==0){
		status = gpio_direction_output(GPIO_LED,0);
	}
	return status;
}
int led_toggle(){
	int err_status;
	led_status = 1-led_status;
 	err_status=gpio_set_value(GPIO_LED,led_status);
	return err_status;
}

void timer_callback(struct timer_list *timer){
	//timer callback;
	//test if mode is auto or manual
	led_toggle();
	
}

static int __init skeleton_init(void)
{//char based driver
	int status=0;
	//init mode to automatic
	mode=0;
	led_status  = 0;
	 //---TEMP INIT BEGIN---
	thermal_zone_dev =  thermal_zone_get_zone_by_name("cpu-thermal");
	if(thermal_zone_dev != NULL)status = thermal_zone_get_temp(thermal_zone_dev,&current_temperature);
	//---TEMP INIT END---
	
	//---LED INIT BEGIN ---
	if(status==0)status=led_init();
	//---LED INIT END---
	
	//---TIMER INIT BEGIN---
	setup_timer(&my_timer,timer_callback,CLOCK_MONOTONIC);
	status = mod_timer(&my_timer,jiffies+msecs_to_jiffies(freq_from_temp(current_temperature)));
	//get initial freq from temp so as to have an adequate fan frequency on startup
	//---TIMER INIT END---

	sysfs_class = class_create(THIS_MODULE, "fan_controller_class");
    sysfs_device = device_create(sysfs_class, NULL, skeleton_dev, NULL, "fan_controller_device");
    if (status == 0) status = device_create_file(sysfs_device, &dev_attr_mode);
 	if (status == 0) status = device_create_file(sysfs_device, &dev_attr_frequency);
}

static void __exit skeleton_exit(void)
{
	pr_info("Linux module skeleton unloaded\n");
	gpio_free(GPIO_LED);
	del_timer(&my_timer);
	pr_info ("Linux module skeleton unloaded\n");
}
module_init(skeleton_init);
module_exit(skeleton_exit);
MODULE_AUTHOR("Simon Corboz <simon.corboz@hes-so.ch>");
MODULE_DESCRIPTION("Module skeleton");
MODULE_LICENSE("GPL");
#include <linux/module.h>	// needed by all modules
#include <linux/moduleparam.h>	// needed by all modules
#include <linux/init.h>		// needed for macros
#include <linux/kernel.h>	// needed for debugging
#include <linux/moduleparam.h>	// needed for module parameters
#include <linux/device.h>//needed for sysfs handling
#include <linux/platform_device.h>//needed for sysfs handling

#define MAX_NAME_CONF_LENGTH 32
#define MAX_DESCR_CONF_LENGTH 128

//define mode
// 0--6--6--4
#define RW 0664
//naming convention for device attribute:
//var_name_ACCESSRIGHTS

struct skeleton_struct {
    int id;
    long ref;
    char descr_buffer[MAX_DESCR_CONF_LENGTH];
    char name_buffer[MAX_NAME_CONF_LENGTH];
};

static struct skeleton_struct complex_val_RW;
static int simple_val_RW;

//----class def
static struct class* sysfs_class;
static struct device* sysfs_device;
//-------------------------------------------------------------------------------------------
//copies an integer value to the user buffer
//SRC simple_val_RW
//FORMAT %d\n
//DST buf
//no buffer boundary checking
//PRE : buffer must have enough space to hold INTEGER_MAX_VALUE with 0 termination
//-------------------------------------------------------------------------------------------
ssize_t sysfs_show_simple_val_RW(struct device* dev, struct device_attribute* attr,char* buf){
    sprintf(buf,"%d\n",simple_val_RW);
    return strlen(buf);
}
//-------------------------------------------------------------------------------------------
//put a char ptr into a signed integer value
//PRE : user buffer must contain a correct integer representation 
//      -> no floating point representation
//      -> no number representation bigger than INTEGER_MAX_INT
//      -> no number representation smaller than INTEGER_MIN_INT
//      -> no non number characters
//-------------------------------------------------------------------------------------------
ssize_t sysfs_store_simple_val_RW(struct device* dev, struct device_attribute* attr,const char* buf,size_t count){
    simple_val_RW = simple_strtol(buf,0,10);  
    return count;
}
DEVICE_ATTR(simple_val_RW,RW,sysfs_show_simple_val_RW,sysfs_store_simple_val_RW);
//-------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------
ssize_t sysfs_show_complex_val_RW(struct device* dev, struct device_attribute* attr,char* buf){
    sprintf(buf,"ID %d REFERENCE %ld NAME %s DESCRIPTION %s\n", complex_val_RW.id,
                                                                complex_val_RW.ref,
                                                                complex_val_RW.name_buffer,
                                                                complex_val_RW.descr_buffer);
    return strlen(buf);
}
//-------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------
ssize_t sysfs_store_complex_val_RW(struct device* dev, struct device_attribute* attr,const char* buf,size_t count){
    sscanf(buf,"%d %ld %s %s",  &complex_val_RW.id,
                                &complex_val_RW.ref,
                                complex_val_RW.name_buffer,
                                complex_val_RW.descr_buffer);
    return count;
}
DEVICE_ATTR(complex_val_RW,RW,sysfs_show_complex_val_RW,sysfs_store_complex_val_RW);

static int __init skeleton_init(void)
{
    pr_info ("Linux module skeleton loaded\n");
    sysfs_class = class_create(THIS_MODULE,"my_custom_class_sysfs");
    sysfs_device = device_create(sysfs_class,NULL,0,NULL,"my_custom_device_sysf");
    int status = 0;
    status = device_create_file(sysfs_device,&dev_attr_simple_val_RW);//dev_attr_NAME is created by the macro DEVICE_ATTR (see line 55)
    status = device_create_file(sysfs_device,&dev_attr_complex_val_RW);//dev_attr_NAME is created by the macro DEVICE_ATTR (see line 81)
    return 0;
}

static void __exit skeleton_exit(void)
{
    //this pattern of ressource destruction could be called
    //in a method that tracks which ressources have been 
    //created
    device_remove_file(sysfs_device,&dev_attr_simple_val_RW);
    device_remove_file(sysfs_device,&dev_attr_complex_val_RW);
    device_destroy(sysfs_class,0);
    class_destroy(sysfs_class);
	pr_info ("Linux module skeleton unloaded\n");
}

module_init (skeleton_init);
module_exit (skeleton_exit);

MODULE_AUTHOR ("Corboz Simon");
MODULE_DESCRIPTION ("Module skeleton");
MODULE_LICENSE ("GPL");
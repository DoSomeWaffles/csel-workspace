#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>        //this is the file structure, file open read close 
#include <linux/cdev.h>      //this is for character device, makes cdev avilable
#include <linux/semaphore.h> //this is for the semaphore
#include <linux/uaccess.h>   //this is for copy_user vice vers

int skeleton_init(void);
void skeleton_exit(void);

#define BUFFER_SIZE 1024
static char device_buffer[BUFFER_SIZE];
struct semaphore sem;
struct cdev *mcdev; //this is the name of my char driver that i will be registering
dev_t dev_num;      //will hold the major number that the kernel gives

#define DEVICENAME "skeleton"

// inode reffers to the actual file on disk
static int skeleton_open(struct inode *inode, struct file *file)
{
    // test semaphore to know if the device is already opened
    if (down_interruptible(&sem) != 0)
    {
        pr_info("skeleton : the device has been opened by some other device, unable to open lock\n");
        return -1;
    }
    pr_info("skeleton : device opened succesfully\n");
    return 0;
}

static int skeleton_close(struct inode *inode, struct file *file)
{
    up(&sem); // release the semaphore
    pr_info("skeleton : device has been closed\n");
    return 0;
}

static ssize_t skeleton_read(struct file *fp, char *buffer, size_t length, loff_t *offset)
{
    int maxbytes;      // maximum bytes that can be read from offset to BUFFER_SIZE
    int bytes_to_read; // gives the number of bytes to read
    int bytes_read;    // number of bytes actually read
    maxbytes = BUFFER_SIZE - *offset;
    if (maxbytes > length)
        bytes_to_read = length;
    else
        bytes_to_read = maxbytes;

    bytes_read = bytes_to_read - copy_to_user(buffer, device_buffer + *offset, bytes_to_read);
    pr_info("skeleton : read operation... read=%d\n", bytes_read);

    *offset += bytes_read;

    return bytes_read;
}

static ssize_t skeleton_write(struct file *fp, const char *buffer, size_t length, loff_t *offset)
{
    int maxbytes;       // maximum bytes that can be read from offset to BUFFER_SIZE
    int bytes_to_write; // gives the number of bytes to write
    int bytes_writen;   // number of bytes actually writen
    maxbytes = BUFFER_SIZE - *offset;
    if (maxbytes > length)
        bytes_to_write = length;
    else
        bytes_to_write = maxbytes;

    bytes_writen = bytes_to_write - copy_from_user(device_buffer + *offset, buffer, bytes_to_write);
    pr_info("skeleton : device has been written %d\n", bytes_writen);
    *offset += bytes_writen;
    return bytes_writen;
}

struct file_operations fops = {
    //these are the file operations provided by our driver 
    .owner = THIS_MODULE,      //prevents unloading when operations are in use
    .open = skeleton_open,     //to open the device
    .write = skeleton_write,   //to write to the device
    .read = skeleton_read,     //to read the device
    .release = skeleton_close, //to close the device
};

int skeleton_init(void)
{
    int ret;
    //we will get the major number dynamically this is recommended please read ldd3
    ret = alloc_chrdev_region(&dev_num, 0, 1, DEVICENAME);
    if (ret < 0)
    {
        pr_info(" skeleton : failed to allocate major number\n");
        return ret;
    }
    else
        pr_info(" skeleton : major number allocated succesful\n");
    mcdev = cdev_alloc(); //create, allocate and initialize our cdev structure
    mcdev->ops = &fops;   //fops stand for our file operations
    mcdev->owner = THIS_MODULE;
    ret = cdev_add(mcdev, dev_num, 1);
    if (ret < 0)
    {
        pr_info("skeleton : device adding to the kerknel failed\n");
        return ret;
    }
    else
        pr_info("skeleton : device additin to the kernel succesful\n");
    sema_init(&sem, 1); //initial value to one

    return 0;
}

void skeleton_exit(void)
{
    cdev_del(mcdev); 
    pr_info(" skeleton : removed the mcdev from kernel\n");

    unregister_chrdev_region(dev_num, 1);
    pr_info(" skeleton : unregistered the device numbers\n");
    pr_info(" skeleton : character driver is exiting\n");
}

MODULE_LICENSE("GPL");
module_init(skeleton_init);
module_exit(skeleton_exit);
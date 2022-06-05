/* skeleton.c */
#include <linux/cdev.h>        /* needed for char device driver */
#include <linux/fs.h>          /* needed for device drivers */
#include <linux/init.h>        /* needed for macros */
#include <linux/kernel.h>      /* needed for debugging */
#include <linux/module.h>      /* needed by all modules */
#include <linux/moduleparam.h> /* needed for module parameters */
#include <linux/slab.h>        /* needed for dynamic memory management */
#include <linux/uaccess.h>     /* needed to copy data to/from user */

static int instances = 3;
module_param(instances, int, 0);

#define BUFFER_SZ 10000
static char **buffers = 0;

// inode reffers to the actual file on disk
static int skeleton_open(struct inode *inode, struct file *file)
{
    // we find major and minor in the inode of the device
    pr_info("skeleton : opening inode... major:%d, minor:%d\n", imajor(inode), iminor(inode));
    // test if minor is a valid device number
    if (iminor(inode) >= instances)
    {
        pr_info("skeleton : invalid minor number\n");
        return -EFAULT;
    }

    // associate the buffer to the device to get the private data
    file->private_data = buffers[iminor(inode)];
    pr_info("skeleton : device opened succesfully\n");
    return 0;
}


static int skeleton_release(struct inode *inode, struct file *file)
{
    pr_info("skeleton: release operation...\n");
    return 0;
}

static ssize_t skeleton_read(struct file *file, char __user *buf, size_t count, loff_t *off)
{
    // compute remaining bytes to copy, update count and pointers
    ssize_t remaining = BUFFER_SZ - (ssize_t)(*off);
    char *ptr = (char *)file->private_data + *off;
    if (count > remaining)
        count = remaining;
    *off += count;

    // copy required number of bytes
    if (copy_to_user(buf, ptr, count) != 0)
        count = -EFAULT;

    pr_info("skeleton: read operation... read=%ld\n", count);

    return count;
}

static ssize_t skeleton_write(struct file *file, const char __user *buf, size_t count, loff_t *off)
{
    // compute remaining space in buffer and update pointers
    ssize_t remaining = BUFFER_SZ - (ssize_t)(*off);

    pr_info("skeleton: at%ld\n", (unsigned long)(*off));

    // check if still remaining space to store additional bytes
    if (count >= remaining)
        count = -EIO;

    // store additional bytes into internal buffer
    if (count > 0)
    {
        char *ptr = file->private_data + *off;
        *off += count;
        ptr[count] = 0; // make sure string is null terminated
        if (copy_from_user(ptr, buf, count))
            count = -EFAULT;
    }

    pr_info("skeleton: write operation... private_data=%p, written=%ld\n", file->private_data, count);
    return count;
}

static struct file_operations skeleton_fops = {
 /* these are the file operations provided by our driver */
    .owner = THIS_MODULE,      /* prevents unloading when operations are in use*/
    .open = skeleton_open,     /* to open the device*/
    .write = skeleton_write,   /* to write to the device*/
    .read = skeleton_read,     /* to read the device*/
    .release = skeleton_release, /* to close the device*/
};

static dev_t skeleton_dev;
static struct cdev skeleton_cdev;

static int __init skeleton_init(void)
{
    int inode, status;
    if (instances <= 0)
        return -EFAULT;
    // dynamic allocation of character device region
    status = alloc_chrdev_region(&skeleton_dev, 0, instances, "mymodule");

    if (status < 0)
    {

        pr_info("skeleton: error %d\n", status);
    }
    else
    {
        cdev_init(&skeleton_cdev, &skeleton_fops);
        skeleton_cdev.owner = THIS_MODULE;
        status = cdev_add(&skeleton_cdev, skeleton_dev, instances);
        buffers = kzalloc(sizeof(char *) * instances, GFP_KERNEL);

        for (inode = 0; inode < instances; inode++)
        {
            buffers[inode] = kzalloc(BUFFER_SZ, GFP_KERNEL);
        }
    }

    pr_info("Linux module skeleton loaded with multiples instances\n");
    pr_info("The number of instances: %d\n", instances);
    return 0;
}

static void __exit skeleton_exit(void)
{
    int inode;
    cdev_del(&skeleton_cdev);
    unregister_chrdev_region(skeleton_dev, 1);

    for (inode = 0; inode < instances; inode++)
        kfree(buffers[inode]);
    kfree(buffers);
    pr_info("Linux module skeleton with multiple instances unloaded\n");
}

module_init(skeleton_init);
module_exit(skeleton_exit);

MODULE_AUTHOR("Daniel Gachet <daniel.gachet@hefr.ch> modified by Denis Rosset <denis.rosset@hes-so.ch> and Simon Corboz <simon.corboz@hes-so.ch>");
MODULE_DESCRIPTION("Module skeleton");
MODULE_LICENSE("GPL");
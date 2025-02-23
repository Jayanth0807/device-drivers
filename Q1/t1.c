#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/errno.h>
#include <asm/current.h>
#include <linux/device.h>

#define MAJORNO 125
#define MINORNO  0
#define CHAR_DEV_NAME "jay"
#define MAX_LENGTH 4096
#define SUCCESS 0
 
static char *char_device_buf;
struct cdev *f_dev;
dev_t mydev;
unsigned count=1,inuse=0;
static struct class *my_class;
int char_dev_open(struct inode *inode, struct file  *file)
{
	if(inuse)
	{
		printk(KERN_INFO "\nDevice busy %s\n",CHAR_DEV_NAME);
		return -EBUSY;
	}
	inuse=1;
	printk(KERN_INFO "Open operation invoked \n");
	return SUCCESS;
}
 
static int char_dev_release(struct inode *inode, struct file *file)
{
	inuse=0;
	printk(KERN_INFO "release operation invoked\n");
	return SUCCESS;
}

static ssize_t char_dev_write(struct file *file, const char *buf, size_t lbuf, loff_t *ppos)
{
	printk(KERN_INFO "write section ------->\n");
	printk(KERN_INFO "Parameters :-\n\t*buf : %s\n\tlbuf : %ls\n\tppos : %d\n",buf,lbuf,(int) *ppos);
	//*ppos = 0;
        int nbytes = lbuf - copy_from_user (char_device_buf + *ppos, buf, lbuf);
        *ppos += nbytes;
	printk (KERN_INFO "\n Rec'vd data from user :  %s , nbytes : %d , *ppos : %d \n",char_device_buf,nbytes,(int)*ppos);
	printk(KERN_INFO "-------------------------------------------------------------------------\n");
	return nbytes;
}

static ssize_t char_dev_read(struct file *file, char *buf, size_t lbuf, loff_t *ppos)
{
 	printk(KERN_INFO "read section -------->\n");
	*ppos = 0;
       	int nbytes = lbuf - copy_to_user (buf, char_device_buf + *ppos, lbuf);
       	*ppos += nbytes; 
       	printk (KERN_INFO "\n Reading nbytes=%d, pos=%d \n",nbytes, (int)*ppos); 
	printk("-------------------------------------------------------------------------\n");
	return 0;
}
 
static struct file_operations char_dev_fops = 
{
	.owner = THIS_MODULE,
	.write = char_dev_write,
	.read = char_dev_read,
	.open = char_dev_open,
	.release = char_dev_release
};
 
static int __init char_dev_init(void)
{
	int ret;

	if (alloc_chrdev_region (&mydev, 0, count, CHAR_DEV_NAME) < 0)
        {  //1
            printk (KERN_ERR "failed to reserve major/minor range\n");
            return -1;
        }

        if (!(f_dev = cdev_alloc ()))
        { //2
            printk (KERN_ERR "cdev_alloc() failed\n");
            unregister_chrdev_region (mydev, count);
            return -1;
        }

        cdev_init(f_dev,&char_dev_fops);//3
					    
        ret=cdev_add(f_dev,mydev,count);//4
        if( ret < 0 )
        {
                printk(KERN_INFO "Error registering device driver\n");
                cdev_del (f_dev);
                unregister_chrdev_region (mydev, count);
        	return -1;
        }

        my_class = class_create(THIS_MODULE->name);//5 extra
							   //
        device_create (my_class, NULL, mydev, NULL, "%s", CHAR_DEV_NAME);//6 extra

        printk(KERN_INFO"\nDevice Registered: %s\n",CHAR_DEV_NAME);
        printk (KERN_INFO "Major number = %d, Minor number = %d\n", MAJOR (mydev),MINOR (mydev));

        char_device_buf =(char *)kmalloc(MAX_LENGTH,GFP_KERNEL);

	return 0;
}
 
static void __exit char_dev_exit(void)
{ 
	cdev_del(f_dev); //5
        class_destroy(my_class);
	unregister_chrdev_region(mydev,1); //6
	kfree(char_device_buf);
	printk(KERN_INFO "\n Driver unregistered \n");
}
 
module_init(char_dev_init);
module_exit(char_dev_exit);
 
MODULE_AUTHOR("jay");
MODULE_DESCRIPTION("Character Device Driver - Test");
MODULE_LICENSE("GPL");


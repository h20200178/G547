/*
 *  main.c - Create an input/output character device
 */

#include<linux/module.h>       /* Specifically, a module */
#include<linux/kernel.h>       /* We're doing kernel work */
#include<linux/types.h>
#include<linux/kdev_t.h>
#include<linux/fs.h>
#include<linux/device.h>
#include<linux/cdev.h>
#include<linux/random.h>	// for get_random_bytes()

#include "main_ioctl.h"
#define SUCCESS 0
#define DEVICE_NAME "imu_char"

static dev_t devt;
static struct cdev c_dev;
static struct class *cls;
int flag = 0;	

  
static int my_open(struct inode *i, struct file *f)	//Opening device 
{
	printk(KERN_INFO "Mychar: open()\n");
	return 0;
}

static int my_close(struct inode *i, struct file *f)	//Closing device file
{
	printk(KERN_INFO "Mychar: close()\n");
	return 0;
}

static ssize_t my_read(struct file *f, char __user *buf, size_t len, loff_t *off)
{
	uint16_t num;
	int retval;
	
	printk(KERN_INFO "read()\n");
	
    	get_random_bytes(&num, 2);	//Random number generator

	if(flag == 1)		//Masking to 10 bits for pressure sensor
	{
		num &= 0x000003FF;
		flag = 0;
	}

    	printk(KERN_INFO "random_num :%d\n",num);
	retval = copy_to_user(buf, &num, sizeof(num));	//Sending data to user space
	if(retval !=0 )
		printk(KERN_INFO "Could not transfer bytes\n");	
	return 0;
}

long device_ioctl(struct file *f,unsigned int ioctl_num,unsigned long ioctl_param)
{
	
	switch(ioctl_num)		//Switch cases to read different parameters
	{
		case GYRO_X_READ:
			my_read(f,(char*)ioctl_param,2,0);
			break;
		case GYRO_Y_READ:
			my_read(f,(char*)ioctl_param,2,0);
			break;
		case GYRO_Z_READ:
			my_read(f,(char*)ioctl_param,2,0);
			break;
		case ACC_X_READ:
			my_read(f,(char*)ioctl_param,2,0);
			break;
		case ACC_Y_READ:
			my_read(f,(char*)ioctl_param,2,0);
			break;
		case ACC_Z_READ:
			my_read(f,(char*)ioctl_param,2,0);
			break;
		case COM_X_READ:
			my_read(f,(char*)ioctl_param,2,0);
			break;
		case COM_Y_READ:
			my_read(f,(char*)ioctl_param,2,0);
			break;
		case COM_Z_READ:
			my_read(f,(char*)ioctl_param,2,0);
			break;
		case PRESSURE_READ:
			flag = 1;
			my_read(f,(char*)ioctl_param,2,0);
			break;
	}
	return SUCCESS;
}



struct file_operations fops = {
	.owner = THIS_MODULE,
        .read = my_read,
        .unlocked_ioctl = device_ioctl,
        .open = my_open,
        .release = my_close     /* a.k.a. close */
};

/*
 * Initialize the module - Register the character device
 */
int __init mychar_init(void)
{
	printk(KERN_INFO "IMU driver registered\n");
	
	// Step 1: Major & Minor number
	
    	if (alloc_chrdev_region(&devt,0,1,"AYUSH") < 0)
	{
		return -1;
	}
	printk(KERN_INFO "<major , minor>:<%d,%d>\n",MAJOR(devt),MINOR(devt));
	
	//Step 2: Device file Creation
	
	if ((cls = class_create(THIS_MODULE, "chardrv")) == NULL)
	{
		unregister_chrdev_region(devt,1);
		return -1;
	}
	
	if (device_create(cls, NULL, devt, NULL, "imu_char") == NULL)
	{
		class_destroy(cls);
		unregister_chrdev_region(devt,1);
		return -1;
	}
	
	//Step 3: cdev Initialisationi
	
	cdev_init(&c_dev, &fops);
		
	if (cdev_add(&c_dev, devt, 1) == -1)
	{
		device_destroy(cls,devt);
		class_destroy(cls);
		unregister_chrdev_region(devt,1);
		return -1;
	}  
	return 0;
}

//Exit module

static void __exit mychar_exit(void)
{
	cdev_del(&c_dev);
	device_destroy(cls,devt);
	class_destroy(cls);
	unregister_chrdev_region(devt,1);
	printk(KERN_INFO "IMU driver unregistered\n");
}

module_init(mychar_init);
module_exit(mychar_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("AYUSH KAPUR");
MODULE_DESCRIPTION("Charachter driver module for IMU sensor");

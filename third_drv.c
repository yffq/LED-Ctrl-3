#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <asm/uaccess.h>
#include <asm/irq.h>
#include <asm/io.h>
#include <asm/arch/regs-gpio.h>
#include <asm/hardware.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/mm.h>

#define MAJOR_NUM 113

static struct class		*thirddrv_class;
static struct class_device	*thirddrv_class_dev;
static struct cdev		*third_drv; 
static char msg;

static volatile unsigned long *gpfcon = NULL;
static volatile unsigned long *gpfdat = NULL;

static int third_drv_open(struct inode *my_inode, struct file *my_file)
{
	/* default to be level 4*/
	printk("third_drv_open\n");
	/* GPF4, 5, 6 correspond respectively LED1, LED2, LED4 */
	/* reset */
	*gpfcon &= ~((0x3<<(4*2)) | (0x3<<(5*2)) | (0x3<<(6*2)));
	/* set, 0x01 means output */
	*gpfcon |= ((0x1<<(4*2)) | (0x1<<(5*2)) | (0x1<<(6*2)));
	
	/* 0 corresponds not lighted LED */
	*gpfdat |= (1<<4) | (1<<5) | (1<<6);

	return 0;
}

static int third_drv_release(struct inode *my_inode, struct file *my_file)
{
	printk("third_drv_release\n");
	*gpfdat |= (1<<4) | (1<<5) | (1<<6);
	return 0;
}

static ssize_t third_drv_read(struct file *my_file, char __user *user_addr, size_t length, loff_t *pos)
{
	int ret;
	ret=access_ok (VERIFY_WRITE, user_addr, length);
	if(ret == 0)
		printk("third_drv_read access_ok failure!\n");
	ret=copy_to_user(user_addr, &msg, length);
	if(ret == 0)
		printk("third_drv_read success!  After reading, msg=%d.\n",msg);
	else
		printk("third_drv_read failure!\n");
	return length;
}

static ssize_t third_drv_write(struct file *my_file, const char __user *user_addr, size_t length, loff_t *pos)
{
	int ret;
	ret=copy_from_user(&msg, user_addr, length);
	if(ret == 0)
		printk("hird_drv_write success!  After writting, msg=%d.\n",msg);
	else
		printk("third_drv_write failure!\n");
	return length;
}

static int third_drv_ioctl(struct inode *my_inode, struct file *my_file, unsigned int cmd, unsigned long arg)
{
	switch(cmd)
	{
		case 0x01:
					*gpfdat |= (1<<4) | (1<<5) | (1<<6);
					*gpfdat &= ~ (1<<4) ;
					break;
		case 0x02:
					*gpfdat |= (1<<4) | (1<<5) | (1<<6);
					*gpfdat &= ~ (1<<5) ;
					break;	
		case 0x03:
					*gpfdat |= (1<<4) | (1<<5) | (1<<6);
					*gpfdat &= ~ (1<<6) ;
					break;	
	}
	return 0;
				
}

 static struct file_operations third_drv_fops=
 {
         .owner  =	THIS_MODULE,
         .open   =	third_drv_open,
         .release=	third_drv_release,
         .ioctl  =	third_drv_ioctl,
         .read   =  third_drv_read,
         .write  =  third_drv_write,
 };

static int __init third_drv_init(void)
{
    int err;
	dev_t devid;
	devid = MKDEV(MAJOR_NUM, 0);
	third_drv = cdev_alloc();
	cdev_init(third_drv, &third_drv_fops);
	err = cdev_add(third_drv, devid, 1);
 	if(err)
		return -1;

	/*old way
	int major;
	major = register_chrdev(0, "third_drv", &third_drv_fops); 
       */

	thirddrv_class = (struct class *)class_create(THIS_MODULE, "thirddrv");
	/* /dev/thirddrv */
	thirddrv_class_dev = (struct class_device *)class_device_create(thirddrv_class, NULL, devid, NULL, "thirddrv");
	/*physical address converted to logical address*/
	gpfcon = (volatile unsigned long *)ioremap(0x56000050,0x16);
	gpfdat = (volatile unsigned long *)ioremap(0x56000054,0x08);

	return 0;
}


static void __exit third_drv_exit(void)
{
	cdev_del(third_drv);

	/*old way
	unregister_chrdev(major, "third_drv");
	*/ 
	
	class_device_unregister(thirddrv_class_dev);
	class_destroy(thirddrv_class);
	iounmap(gpfcon);
}

module_init(third_drv_init);
module_exit(third_drv_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("John");
MODULE_DESCRIPTION("LED Ctrl 3");
MODULE_VERSION("1.00");

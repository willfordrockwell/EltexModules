// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * my_module.c - Demonstrating simple module for linux kernel
 */
#include <linux/uaccess.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/init.h>
#include <linux/fs.h>

#define MODULE_NAME "my_module"

#define DRIVER_NAME "willford"
#define CLASS_NAME "test"
#define DEVICE_NAME "test_module"
#define BUFF_LEN 80
#define MAX_MINOR 1
#define BASE_MINOR 0

#define SUCCESS 0
#define END_BUFF 0
#define FAILURE -1

#define PARENT NULL
#define DRIVER_DATA NULL

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ilia Gaevsky <willfordworks@gmail.com>");
MODULE_DESCRIPTION("A simple module with character device");

static int open_device(struct inode *, struct file *);
static int release_device(struct inode *, struct file *);
static int uevent_device(struct device *, struct kobj_uevent_env *);
static ssize_t read_device(struct file *, char *, size_t, loff_t *);
static ssize_t write_device(struct file *, const char *, size_t, loff_t *);

static int major;

static dev_t dev;

static char msg[BUFF_LEN];

static struct class *class;
static struct device *devices[MAX_MINOR];
static struct cdev cdevs[MAX_MINOR];

const struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = open_device,
	.release = release_device,
	.read = read_device,
	.write = write_device
};

static int __init init_chardev_module(void)
{
	int ret, i, j;

	ret = alloc_chrdev_region(&dev, BASE_MINOR, MAX_MINOR, DRIVER_NAME);
	if (ret > SUCCESS) {
		pr_alert(MODULE_NAME ": Cannot alloc region error: %d", ret);
		return ret;
	}
	major = MAJOR(dev);
	pr_info(MODULE_NAME ": Device's major: %d", major);
	class = class_create(THIS_MODULE, CLASS_NAME);
	if (class == NULL) {
		unregister_chrdev_region(dev, MAX_MINOR);
		pr_alert(MODULE_NAME ": Cannor create class");
		return FAILURE;
	}
	class->dev_uevent = uevent_device;
	pr_info(MODULE_NAME ": Class created");
	for (i = 0; i < MAX_MINOR; i++) {
		devices[i] = device_create(class, PARENT, MKDEV(major, i),
					   DRIVER_DATA, DEVICE_NAME "%d", i);
		if (devices[i] == NULL) {
			for (j = 0; j < i; j++)
				device_destroy(class, MKDEV(major, j));
			pr_alert(MODULE_NAME ": Error create device");
			class_destroy(class);
			unregister_chrdev_region(dev, MAX_MINOR);
			return FAILURE;
		}
		pr_info(MODULE_NAME ": Device created");
		cdev_init(&(cdevs[i]), &fops);
		if (cdev_add(&(cdevs[i]), MKDEV(major, i), 1) < SUCCESS) {
			pr_alert(MODULE_NAME ": Error create cdev");
			for (j = 0; j <= i; j++) {
				if (j != i)
					cdev_del(&(cdevs[j]));
				device_destroy(class, MKDEV(major, j));
			}
			class_destroy(class);
			unregister_chrdev_region(dev, MAX_MINOR);
			return FAILURE;
		}
		pr_info(MODULE_NAME ": Filename: /dev/" DEVICE_NAME "%d", i);
	}
	return SUCCESS;
}

static void __exit cleanup_chardev_module(void)
{
	int i;

	for (i = 0; i < MAX_MINOR; i++) {
		cdev_del(&(cdevs[i]));
		device_destroy(class, MKDEV(major, i));
	}
	unregister_chrdev_region(major, MAX_MINOR);
	class_destroy(class);
	pr_info(MODULE_NAME ": Char device unregistered\n");
}

static int uevent_device(struct device *dev, struct kobj_uevent_env *env)
{
	add_uevent_var(env, "DEVMODE=%#o", 0666);
	return SUCCESS;
}

static int open_device(struct inode *inode, struct file *filp)
{
	pr_info(MODULE_NAME ": Opened");
	return SUCCESS;
}

static int release_device(struct inode *inode, struct file *filp)
{
	pr_info(MODULE_NAME ": Released");
	return SUCCESS;
}

static ssize_t read_device(struct file *filp, char __user *buff, size_t len,
			   loff_t *offset)
{
	static int count;

	len = (len > strlen(buff)) ? strlen(buff) : len;
	sprintf(msg, "%d's hello\n", ++count);
	if (copy_to_user(buff, msg, len)) {
		pr_alert(MODULE_NAME "Error reading");
		return -EFAULT;
	}
	return SUCCESS;
}

static ssize_t write_device(struct file *filp, const char *buff, size_t len,
			    loff_t *offset)
{
	pr_info(MODULE_NAME ": Writed");
	return SUCCESS;
}

module_init(init_chardev_module);
module_exit(cleanup_chardev_module);

// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * my_module.c - Demonstrating tha simplest module for linux kernel
 */

#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ilia Gaevsky <willfordworks@gmail.com>");
MODULE_DESCRIPTION("A simple hello-world-module");

static int __init init_hello_module(void)
{
	printk(KERN_INFO "Hello module\n");
	return 0;
}

static void __exit cleanup_hello_module(void)
{
	printk(KERN_INFO "Bye module\n");
}

module_init(init_hello_module);
module_exit(cleanup_hello_module);

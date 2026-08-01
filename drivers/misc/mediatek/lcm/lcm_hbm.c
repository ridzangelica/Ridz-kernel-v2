// SPDX-License-Identifier: GPL-2.0
/*
 * lcm_hbm.c - High Brightness Mode toggle for Ridz kernel
 *
 * Adds a single runtime switch, /sys/kernel/lcm_hbm/enable, that the
 * panel drivers (nt36525b / hx83102d / icnl9911c / ft8006s variants
 * used on angelica/dandelion - Redmi 9A/9C) read before applying their
 * hardcoded "level * 72 / 100" software brightness cap.
 *
 * enable = 0 (default) -> stock behaviour, 72% cap, unchanged.
 * enable = 1           -> cap is skipped, backlight DCS command 0x51 is
 *                         allowed to reach the panel's real maximum DBV.
 *
 * This does not add a new DCS command sequence, it only removes a
 * software throttle that already existed in every panel driver in this
 * tree, so it stays within the brightness range the panel vendor
 * already validated. Because it still increases sustained backlight
 * LED current/heat versus stock, userspace controlling this node
 * should only enable it briefly (e.g. outdoors/direct sunlight) and
 * turn it back off afterwards.
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>

int lcm_hbm_enable;
EXPORT_SYMBOL(lcm_hbm_enable);

static struct kobject *lcm_hbm_kobj;

static ssize_t enable_show(struct kobject *kobj,
	struct kobj_attribute *attr, char *buf)
{
	return sprintf(buf, "%d\n", lcm_hbm_enable);
}

static ssize_t enable_store(struct kobject *kobj,
	struct kobj_attribute *attr, const char *buf, size_t count)
{
	int val;
	int ret;

	ret = kstrtoint(buf, 10, &val);
	if (ret < 0)
		return ret;

	lcm_hbm_enable = (val != 0) ? 1 : 0;

	return count;
}

static struct kobj_attribute lcm_hbm_enable_attr =
	__ATTR(enable, 0664, enable_show, enable_store);

static struct attribute *lcm_hbm_attrs[] = {
	&lcm_hbm_enable_attr.attr,
	NULL,
};

static struct attribute_group lcm_hbm_attr_group = {
	.attrs = lcm_hbm_attrs,
};

static int __init lcm_hbm_init(void)
{
	int ret;

	lcm_hbm_kobj = kobject_create_and_add("lcm_hbm", kernel_kobj);
	if (!lcm_hbm_kobj)
		return -ENOMEM;

	ret = sysfs_create_group(lcm_hbm_kobj, &lcm_hbm_attr_group);
	if (ret) {
		kobject_put(lcm_hbm_kobj);
		lcm_hbm_kobj = NULL;
	}

	return ret;
}

static void __exit lcm_hbm_exit(void)
{
	if (lcm_hbm_kobj) {
		sysfs_remove_group(lcm_hbm_kobj, &lcm_hbm_attr_group);
		kobject_put(lcm_hbm_kobj);
	}
}

late_initcall(lcm_hbm_init);
module_exit(lcm_hbm_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Ridz kernel LCM HBM/brightness-unlock toggle");

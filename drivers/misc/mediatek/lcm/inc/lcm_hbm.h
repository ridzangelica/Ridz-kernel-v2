/* SPDX-License-Identifier: GPL-2.0 */
/*
 * lcm_hbm.h - High Brightness Mode toggle for Ridz kernel
 *
 * Every panel driver in this tree caps the Android backlight level to
 * 72% of the panel's real DBV (Display Brightness Value) range before
 * writing it to the panel via DCS command 0x51:
 *
 *     level = level * 72 / 100;
 *
 * That 28% of headroom is inside the panel's own validated operating
 * range (it is not an overclock), it is simply never used by stock
 * firmware. lcm_hbm_enable exposes that headroom to userspace via
 * /sys/kernel/lcm_hbm/enable so it can be used as a "sunlight boost" /
 * HBM-style brightness mode on demand.
 */

#ifndef _LCM_HBM_H_
#define _LCM_HBM_H_

/* 0 = stock 72% cap (default), 1 = allow full DBV range */
extern int lcm_hbm_enable;

#endif /* _LCM_HBM_H_ */

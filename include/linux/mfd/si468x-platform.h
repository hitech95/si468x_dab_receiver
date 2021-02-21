/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * include/media/si468x-platform.h -- Platform data specific definitions
 *
 * Copyright (C) 2020 HTL Steyr - Austria
 * Copyright (C) 2020 Franz Parzer
 * based on the si476x driver of 
 * Copyright (C) 2013 Andrey Smirnov
 *
 * Author: Franz Parzer <rpi-receiver@htl-steyr.ac.at>
 * Author: Andrey Smirnov <andrew.smirnov@gmail.com>
 */

#ifndef __SI468X_PLATFORM_H__
#define __SI468X_PLATFORM_H__

enum si468x_xmode {
	SI468X_XMODE_POWER_DOWN			= 0,
	SI468X_XMODE_CRYSTAL			= 1,
	SI468X_XMODE_EXT_CLOCK_SINGLE_ENDED	= 2,
	SI468X_XMODE_EXT_CLOCK_DIFFERENTIAL	= 3,
};

enum si468x_func {
	SI468X_FUNC_MINI_BOOT    = 0,
	SI468X_FUNC_BOOTLOADER   = 1,
	SI468X_FUNC_AM_RECEIVER  = 2,
	SI468X_FUNC_FM_RECEIVER  = 3,
	SI468X_FUNC_DAB_RECEIVER = 4,
};

/**
 * @clk_frequency: oscillator's crystal frequency
 * @tr_size: oscillator core gain is controlled by both TR_SIZE and IBIAS
 * @ibias:   oscillator core gain is controlled by both TR_SIZE and IBIAS
 * @xcload:  Selects the amount of additional on-chip capacitance to
 *          be connected between XTAL1 and gnd and between XTAL2 and
 *          GND. Value will be layout dependent. Range is 0–0x3F i.e.
 *          (0–24 pF)
 */
struct si468x_power_up_args {
	enum si468x_xmode       xmode;
	enum si468x_func        func;
	u32  clk_frequency;
	u8   tr_size;
	u32  ibias;
	u32  xcload;
};

#endif /* __SI468X_PLATFORM_H__ */

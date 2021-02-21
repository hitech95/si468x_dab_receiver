/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * include/media/drv-intf/si468x.h -- Common definitions for si468x driver
 *
 * Copyright (C) 2020 HTL Steyr - Austria
 * Copyright (C) 2020 Franz Parzer
 * based on the si476x driver of 
 * Copyright (C) 2012 Innovative Converged Devices(ICD)
 * Copyright (C) 2013 Andrey Smirnov
 *
 * Author: Franz Parzer <rpi-receiver@htl-steyr.ac.at>
 * Author: Andrey Smirnov <andrew.smirnov@gmail.com>
 */

#ifndef SI468X_H
#define SI468X_H

#include <linux/types.h>
#include <linux/videodev2.h>

#include <linux/mfd/si468x-reports.h>

enum si468x_ctrl_id {
	/* we assume, that SI476X and SI468X do not coexist */
	V4L2_CID_SI468X_RSSI_THRESHOLD	= (V4L2_CID_USER_SI476X_BASE + 1),
	V4L2_CID_SI468X_SNR_THRESHOLD	= (V4L2_CID_USER_SI476X_BASE + 2),
	V4L2_CID_SI468X_MAX_TUNE_ERROR	= (V4L2_CID_USER_SI476X_BASE + 3),
};

#endif /* SI468X_H*/

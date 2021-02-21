// SPDX-License-Identifier: GPL-2.0-only
/*
 * drivers/media/radio/radio-si468x.c -- V4L2 driver for SI468X chips
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

#include <linux/module.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/slab.h>
#include <linux/atomic.h>
#include <linux/videodev2.h>
#include <linux/mutex.h>
#include <linux/debugfs.h>
#include <media/v4l2-common.h>
#include <media/v4l2-ioctl.h>
#include <media/v4l2-ctrls.h>
#include <media/v4l2-event.h>
#include <media/v4l2-device.h>

#include <media/drv-intf/si468x.h>
#include <linux/mfd/si468x-core.h>

#define DRIVER_NAME "si468x-radio"
#define DRIVER_CARD "SI468x AM/FM/DAB Receiver"

enum si468x_freq_bands {
	SI468X_BAND_AM,
	SI468X_BAND_FM,
	SI468X_BAND_DAB,
};

static const struct v4l2_frequency_band si468x_bands[] = {
	[SI468X_BAND_AM] = {
		.type		= V4L2_TUNER_RADIO,
		.index		= SI468X_BAND_AM,
		.capability	= V4L2_TUNER_CAP_LOW
		| V4L2_TUNER_CAP_FREQ_BANDS,
		.rangelow	= 0.52 * FREQ_MUL,
		.rangehigh	= 1.71 * FREQ_MUL,
		.modulation	= V4L2_BAND_MODULATION_AM,
	},
	[SI468X_BAND_FM] = {
		.type		= V4L2_TUNER_RADIO,
		.index		= SI468X_BAND_FM,
		.capability	= V4L2_TUNER_CAP_LOW
		| V4L2_TUNER_CAP_STEREO
		| V4L2_TUNER_CAP_RDS
		| V4L2_TUNER_CAP_RDS_BLOCK_IO
		| V4L2_TUNER_CAP_FREQ_BANDS,
		.rangelow	=  87.5 * FREQ_MUL,
		.rangehigh	= 107.9 * FREQ_MUL,
		.modulation	= V4L2_BAND_MODULATION_FM,
	},
	[SI468X_BAND_DAB] = {
		.type		= V4L2_TUNER_RADIO,
		.index		= SI468X_BAND_DAB,
		.capability	= V4L2_TUNER_CAP_LOW
		| V4L2_TUNER_CAP_STEREO
		| V4L2_TUNER_CAP_FREQ_BANDS,
		.rangelow	= 168.16 * FREQ_MUL,
		.rangehigh	= 239.20 * FREQ_MUL,
		.modulation	= V4L2_BAND_MODULATION_FM,
	},
};

static inline bool si468x_radio_freq_is_inside_of_the_band(u32 freq, int band)
{
	return freq >= si468x_bands[band].rangelow &&
		freq <= si468x_bands[band].rangehigh;
}

static inline bool si468x_radio_range_is_inside_of_the_band(u32 low, u32 high,
							    int band)
{
	return low  >= si468x_bands[band].rangelow &&
		high <= si468x_bands[band].rangehigh;
}

static int si468x_radio_s_ctrl(struct v4l2_ctrl *ctrl);
static int si468x_radio_g_volatile_ctrl(struct v4l2_ctrl *ctrl);

static const struct v4l2_ctrl_ops si468x_ctrl_ops = {
	.g_volatile_ctrl	= si468x_radio_g_volatile_ctrl,
	.s_ctrl			= si468x_radio_s_ctrl,
};

enum si468x_ctrl_idx {
	SI468X_IDX_RSSI_THRESHOLD,
	SI468X_IDX_SNR_THRESHOLD,
	SI468X_IDX_MAX_TUNE_ERROR,
};

static struct v4l2_ctrl_config si468x_ctrls[] = {

	/*
	 * SI468X during its station seeking(or tuning) process uses several
	 * parameters to detrmine if "the station" is valid:
	 *
	 *	- Signal's SNR(in dBuV) must be lower than
	 *	#V4L2_CID_SI468X_SNR_THRESHOLD
	 *	- Signal's RSSI(in dBuV) must be greater than
	 *	#V4L2_CID_SI468X_RSSI_THRESHOLD
	 *	- Signal's frequency deviation(in units of 2ppm) must not be
	 *	more than #V4L2_CID_SI468X_MAX_TUNE_ERROR
	 */
	[SI468X_IDX_RSSI_THRESHOLD] = {
		.ops	= &si468x_ctrl_ops,
		.id	= V4L2_CID_SI468X_RSSI_THRESHOLD,
		.name	= "Valid RSSI Threshold",
		.type	= V4L2_CTRL_TYPE_INTEGER,
		.min	= -128,
		.max	= 127,
		.step	= 1,
	},
	[SI468X_IDX_SNR_THRESHOLD] = {
		.ops	= &si468x_ctrl_ops,
		.id	= V4L2_CID_SI468X_SNR_THRESHOLD,
		.type	= V4L2_CTRL_TYPE_INTEGER,
		.name	= "Valid SNR Threshold",
		.min	= -128,
		.max	= 127,
		.step	= 1,
	},
	[SI468X_IDX_MAX_TUNE_ERROR] = {
		.ops	= &si468x_ctrl_ops,
		.id	= V4L2_CID_SI468X_MAX_TUNE_ERROR,
		.type	= V4L2_CTRL_TYPE_INTEGER,
		.name	= "Max Tune Errors",
		.min	= 0,
		.max	= 126 * 2,
		.step	= 2,
	},
};

struct si468x_radio;

/**
 * struct si468x_radio_ops - vtable of tuner functions
 *
 * This table holds pointers to functions implementing particular
 * operations depending on the mode in which the tuner chip was
 * configured to start in. If the function is not supported
 * corresponding element is set to #NULL.
 *
 * @tune_freq: Tune chip to a specific frequency
 * @seek_start: Star station seeking
 * @rsq_status: Get Received Signal Quality(RSQ) status
 * @rds_blckcnt: Get received RDS blocks count
 * @acf_status: Get the status of Automatically Controlled Features(ACF)
 * @agc_status: Get Automatic Gain Control(AGC) status
 */
struct si468x_radio_ops {
	int (*tune_freq)(struct si468x_core *, struct si468x_tune_freq_args *);
	int (*seek_start)(struct si468x_core *, const struct v4l2_hw_freq_seek *,
			  struct si468x_tune_freq_args *);
	int (*rsq_status)(struct si468x_core *, struct si468x_rsq_status_args *,
			  struct si468x_rsq_status_report *);
	int (*rds_blckcnt)(struct si468x_core *, bool,
			   struct si468x_rds_blockcount_report *);

	int (*acf_status)(struct si468x_core *,
			  struct si468x_acf_status_report *);
	int (*agc_status)(struct si468x_core *,
			  struct si468x_agc_status_report *);
};

static struct si468x_dab_frequency dab_freq_list[] = {
	{ .frequency = 174928, .name = "5A", },
	{ .frequency = 176640, .name = "5B", },
	{ .frequency = 178352, .name = "5C", },
	{ .frequency = 180064, .name = "5D", },
	{ .frequency = 181936, .name = "6A", },
	{ .frequency = 183648, .name = "6B", },
	{ .frequency = 185360, .name = "6C", },
	{ .frequency = 187072, .name = "6D", },
	{ .frequency = 188928, .name = "7A", },
	{ .frequency = 190640, .name = "7B", },
	{ .frequency = 192352, .name = "7C", },
	{ .frequency = 194064, .name = "7D", },
	{ .frequency = 195936, .name = "8A", },
	{ .frequency = 197648, .name = "8B", },
	{ .frequency = 199360, .name = "8C", },
	{ .frequency = 201072, .name = "8D", },
	{ .frequency = 202928, .name = "9A", },
	{ .frequency = 204640, .name = "9B", },
	{ .frequency = 206352, .name = "9C", },
	{ .frequency = 208064, .name = "9D", },
	{ .frequency = 209936, .name = "10A", },
	{ .frequency = 210096, .name = "10N", },
	{ .frequency = 211648, .name = "10B", },
	{ .frequency = 213360, .name = "10C", },
	{ .frequency = 215072, .name = "10D", },
	{ .frequency = 216928, .name = "11A", },
	{ .frequency = 217088, .name = "11N", },
	{ .frequency = 218640, .name = "11B", },
	{ .frequency = 220352, .name = "11C", },
	{ .frequency = 222064, .name = "11D", },
	{ .frequency = 223936, .name = "12A", },
	{ .frequency = 224096, .name = "12N", },
	{ .frequency = 225648, .name = "12B", },
	{ .frequency = 227360, .name = "12C", },
	{ .frequency = 229072, .name = "12D", },
	{ .frequency = 230784, .name = "13A", },
	{ .frequency = 232496, .name = "13B", },
	{ .frequency = 234208, .name = "13C", },
	{ .frequency = 235776, .name = "13D", },
	{ .frequency = 237488, .name = "13E", },
	{ .frequency = 239200, .name = "13F", },
};

static struct si468x_dab_frequency loaded_dab_freq_list[SI468X_DAB_MAX_FREQUENCIES] = {};

/**
 * struct si468x_radio - radio device
 *
 * @v4l2dev: Pointer to V4L2 device created by V4L2 subsystem
 * @videodev: Pointer to video device created by V4L2 subsystem
 * @ctrl_handler: V4L2 controls handler
 * @load_firmware_async: Worker that loads firmware to the device.
 * @core: Pointer to underlying core device
 * @ops: Vtable of functions. See struct si468x_radio_ops for details
 * @debugfs: pointer to &strucd dentry for debugfs
 * @audmode: audio mode, as defined for the rxsubchans field
 *	     at videodev2.h
 *
 * core structure is the radio device is being used
 */
struct si468x_radio {
	struct v4l2_device v4l2dev;
	struct video_device videodev;
	struct v4l2_ctrl_handler ctrl_handler;

	struct work_struct load_firmware_async;

	struct si468x_core *core;
	/* This field should not be accessed unless core lock is held */
	const struct si468x_radio_ops *ops;

	struct dentry	*debugfs;
	u32 audmode;
};

static inline struct si468x_radio *v4l2_dev_to_radio(struct v4l2_device *d)
{
	return container_of(d, struct si468x_radio, v4l2dev);
}

static inline struct si468x_radio *
v4l2_ctrl_handler_to_radio(struct v4l2_ctrl_handler *d)
{
	return container_of(d, struct si468x_radio, ctrl_handler);
}

/* *_TUNE_FREQ family of commands accept frequency in multiples of
    10kHz */
static inline u32 hz_to_si468x(struct si468x_core *core, u32 freq)
{
	u32 result;
	int cnt = 0;
	u32 diff, closest = 0xffffffff;

	switch (core->power_up_parameters.func) {
	case SI468X_FUNC_AM_RECEIVER:
		result = freq / 1000;
		break;
	case SI468X_FUNC_FM_RECEIVER:
	default:
		result = freq / 10000;
		break;
	case SI468X_FUNC_DAB_RECEIVER:
		freq = freq / 1000; /* DAB uses kHz */
		while (loaded_dab_freq_list[cnt].frequency) {
			diff = abs(freq - loaded_dab_freq_list[cnt].frequency);
			if (diff < closest) {
				closest = diff;
				result = loaded_dab_freq_list[cnt].frequency;
			}
			cnt++;
		}
		break;
	}

	return result;
}

static inline u32 si468x_to_hz(struct si468x_core *core, u32 freq)
{
	int result;

	switch (core->power_up_parameters.func) {
	case SI468X_FUNC_AM_RECEIVER:
		result = freq * 1000;
		break;
	case SI468X_FUNC_FM_RECEIVER:
	default:
		result = freq * 10000;
		break;
	case SI468X_FUNC_DAB_RECEIVER:
		result = freq * 1000;
		break;
	}

	return result;
}

/* Since the V4L2_TUNER_CAP_LOW flag is supplied, V4L2 subsystem
 * mesures frequency in 62.5 Hz units */

static inline u32 hz_to_v4l2(u32 freq)
{
	return (freq * 10) / 625;
}

static inline u32 v4l2_to_hz(u32 freq)
{
	return (freq * 625) / 10;
}

static inline u32 v4l2_to_si468x(struct si468x_core *core, u32 freq)
{
	return hz_to_si468x(core, v4l2_to_hz(freq));
}

static inline u32 si468x_to_v4l2(struct si468x_core *core, u32 freq)
{
	return hz_to_v4l2(si468x_to_hz(core, freq));
}

/*
 * si468x_vidioc_querycap - query device capabilities
 */
static int si468x_radio_querycap(struct file *file, void *priv,
				 struct v4l2_capability *capability)
{
	struct si468x_radio *radio = video_drvdata(file);

	strscpy(capability->driver, radio->v4l2dev.name,
		sizeof(capability->driver));
	strscpy(capability->card, DRIVER_CARD, sizeof(capability->card));
	snprintf(capability->bus_info, sizeof(capability->bus_info),
		 "platform:%s", radio->v4l2dev.name);

	capability->device_caps = V4L2_CAP_TUNER
		| V4L2_CAP_RADIO
		| V4L2_CAP_HW_FREQ_SEEK
		| V4L2_CAP_RDS_CAPTURE
		| V4L2_CAP_READWRITE;

	capability->capabilities = capability->device_caps
		| V4L2_CAP_DEVICE_CAPS;
	return 0;
}

static int si468x_radio_enum_freq_bands(struct file *file, void *priv,
					struct v4l2_frequency_band *band)
{
	int err;
	struct si468x_radio *radio = video_drvdata(file);

	if (band->tuner != 0)
		return -EINVAL;

	if (band->index < ARRAY_SIZE(si468x_bands)) {
		switch (band->index) {
		case SI468X_BAND_AM:
			if (radio->core->si468x_device_info->has_am) {
				*band = si468x_bands[band->index];
				err = 0;
			} else {
				err = -EINVAL;
			}
			break;
		case SI468X_BAND_FM:
			*band = si468x_bands[band->index];
			err = 0;
			break;
		case SI468X_BAND_DAB:
			if (radio->core->si468x_device_info->has_dab) {
				*band = si468x_bands[band->index];
				err = 0;
			} else {
				err = -EINVAL;
			}
			break;
		default:
			err = -EINVAL;
		}
	} else {
		err = -EINVAL;
	}

	return err;
}

static int si468x_radio_g_tuner(struct file *file, void *priv,
				struct v4l2_tuner *tuner)
{
	int err;
	struct si468x_rsq_status_report rsq_report;
	struct si468x_acf_status_report acf_report;
	struct si468x_rds_status_report rds_report;
	struct si468x_radio *radio = video_drvdata(file);

	struct si468x_rsq_status_args args = {
		.rsqack		= false,
		.digradack	= false,
		.attune		= false,
		.cancel		= false,
		.fiberrack	= false,
		.stcack		= false,
	};

	if (tuner->index != 0)
		return -EINVAL;

	tuner->type       = V4L2_TUNER_RADIO;
	tuner->capability = V4L2_TUNER_CAP_LOW /* Measure frequencies
						 * in multiples of
						 * 62.5 Hz */
		| V4L2_TUNER_CAP_STEREO
		| V4L2_TUNER_CAP_HWSEEK_BOUNDED
		| V4L2_TUNER_CAP_HWSEEK_WRAP
		| V4L2_TUNER_CAP_HWSEEK_PROG_LIM;

	si468x_core_lock(radio->core);

	tuner->rangelow = si468x_bands[SI468X_BAND_FM].rangelow;
	tuner->rangehigh = si468x_bands[SI468X_BAND_FM].rangehigh;
	if (radio->core->si468x_device_info->has_am)
		tuner->rangelow = si468x_bands[SI468X_BAND_AM].rangelow;
	if (radio->core->si468x_device_info->has_dab)
		tuner->rangehigh = si468x_bands[SI468X_BAND_DAB].rangehigh;

	if (radio->core->si468x_device_info->has_am &&
	    radio->core->si468x_device_info->has_dab)
		strscpy(tuner->name, "AM/FM/DAB", sizeof(tuner->name));
	else if (radio->core->si468x_device_info->has_am)
		strscpy(tuner->name, "AM/FM", sizeof(tuner->name));
	else if (radio->core->si468x_device_info->has_dab)
		strscpy(tuner->name, "FM/DAB", sizeof(tuner->name));
	else
		strscpy(tuner->name, "FM", sizeof(tuner->name));

	tuner->rxsubchans = V4L2_TUNER_SUB_MONO;
	tuner->capability |= V4L2_TUNER_CAP_RDS
		| V4L2_TUNER_CAP_RDS_BLOCK_IO
		| V4L2_TUNER_CAP_FREQ_BANDS;
	tuner->audmode = radio->audmode;
	tuner->afc = 1;

	err = radio->ops->rsq_status(radio->core,
				     &args, &rsq_report);
	if (err < 0)
		tuner->signal = 0;
	else
		/*
		 * tuner->signal value range: 0x0000 .. 0xFFFF,
		 * rsq_report.rssi: -128 .. 127
		 */
		tuner->signal = (rsq_report.rssi + 128) * 257;

	if (radio->ops->acf_status) {
		err = radio->ops->acf_status(radio->core, &acf_report);
		if (!(err < 0)) {
			tuner->audmode = acf_report.pilot ?
						V4L2_TUNER_MODE_STEREO :
						V4L2_TUNER_MODE_MONO;
			if (acf_report.pilot)
				tuner->rxsubchans |= V4L2_TUNER_SUB_STEREO;
		}
	}

	if (radio->core->power_up_parameters.func == SI468X_FUNC_FM_RECEIVER) {
		err = si468x_core_cmd_fm_rds_status(radio->core,
				true, false, false, &rds_report);
		if (!(err < 0))
			if (rds_report.rdssync)
				tuner->rxsubchans |= V4L2_TUNER_SUB_RDS;
	}

	si468x_core_unlock(radio->core);

	return (err < 0) ? err : 0;
}

static int si468x_radio_s_tuner(struct file *file, void *priv,
				const struct v4l2_tuner *tuner)
{
	struct si468x_radio *radio = video_drvdata(file);

	if (tuner->index != 0)
		return -EINVAL;

	if (tuner->audmode == V4L2_TUNER_MODE_MONO ||
	    tuner->audmode == V4L2_TUNER_MODE_STEREO)
		radio->audmode = tuner->audmode;
	else
		radio->audmode = V4L2_TUNER_MODE_STEREO;

	return 0;
}

static int si468x_radio_init_vtable(struct si468x_radio *radio,
				    enum si468x_func func)
{
	static const struct si468x_radio_ops am_ops = {
		.tune_freq		= si468x_core_cmd_am_tune_freq,
		.seek_start		= si468x_core_cmd_am_seek_start,
		.rsq_status		= si468x_core_cmd_am_rsq_status,
		.rds_blckcnt		= NULL,
		.acf_status		= si468x_core_cmd_am_acf_status,
		.agc_status		= NULL,
	};

	static const struct si468x_radio_ops fm_ops = {
		.tune_freq		= si468x_core_cmd_fm_tune_freq,
		.seek_start		= si468x_core_cmd_fm_seek_start,
		.rsq_status		= si468x_core_cmd_fm_rsq_status,
		.rds_blckcnt		= si468x_core_cmd_fm_rds_blockcount,
		.acf_status		= si468x_core_cmd_fm_acf_status,
		.agc_status		= si468x_core_cmd_agc_status,
	};

	static const struct si468x_radio_ops dab_ops = {
		.tune_freq		= si468x_core_cmd_dab_tune_freq,
		.seek_start		= si468x_core_cmd_dab_seek_start,
		.rsq_status		= si468x_core_cmd_dab_rsq_status,
		.rds_blckcnt		= NULL,
		.acf_status		= si468x_core_cmd_dab_acf_status,
		.agc_status		= NULL,
	};

	switch (func) {
	case SI468X_FUNC_AM_RECEIVER:
		radio->ops = &am_ops;
		return 0;

	case SI468X_FUNC_FM_RECEIVER:
		radio->ops = &fm_ops;
		return 0;

	case SI468X_FUNC_DAB_RECEIVER:
		radio->ops = &dab_ops;
		return 0;

	default:
		WARN(1, "Unexpected tuner function value\n");
		return -EINVAL;
	}
}

static int si468x_radio_dab_load_valid_frequencies(struct si468x_radio *radio,
						   struct si468x_tune_freq_args *args)
{
	int err;
	int i, cnt = 0;
	struct si468x_rsq_status_report rsq_report;
	struct si468x_rsq_status_args rsq_args = {
		.rsqack		= false,
		.digradack	= false,
		.attune		= false,
		.cancel		= false,
		.fiberrack	= false,
		.stcack		= false,
	};

	err = si468x_core_cmd_dab_set_freq_list(
				radio->core,
				dab_freq_list,
				ARRAY_SIZE(dab_freq_list),
				SI468X_DAB_MAX_FREQUENCIES);
	if (err < 0)
		return err;
	memset(loaded_dab_freq_list, 0, sizeof(loaded_dab_freq_list));
	for (i = 0; i < ARRAY_SIZE(dab_freq_list); i++) {
		args->dab_freq_list = dab_freq_list;
		args->freq = dab_freq_list[i].frequency;
		err = radio->ops->tune_freq(radio->core, args);
		if (err < 0)
			return err;
		err = radio->ops->rsq_status(radio->core,
					     &rsq_args, &rsq_report);
		if (!(err < 0)) {
			dab_freq_list[i].is_valid = rsq_report.valid;
			if (rsq_report.valid) {
				loaded_dab_freq_list[cnt].frequency =
				dab_freq_list[i].frequency;
				cnt++;
			}
		}
	}

	if (cnt == 0)
		return -EINVAL;

	err = regmap_update_bits(radio->core->regmap_dab,
				 SI468X_PROP_DAB_EVENT_INTERRUPT_SOURCE,
				 SI468X_PROP_SRVLIST_INTEN_MASK,
				 SI468X_PROP_SRVLIST_INTEN);
	if (err < 0)
		return err;
	return si468x_core_cmd_dab_set_freq_list(
				radio->core,
				loaded_dab_freq_list,
				cnt,
				SI468X_DAB_MAX_FREQUENCIES);
}

static int si468x_radio_dab_get_ensembles(struct si468x_radio *radio,
					  struct si468x_tune_freq_args *args)
{
	atomic_set(&radio->core->dab_full_scan, 1);
	args->dab_freq_list = loaded_dab_freq_list;
	args->freq = loaded_dab_freq_list[0].frequency;
	return radio->ops->tune_freq(radio->core, args);
/* wait for list in si468x_core_new_digital_service_list */
}

static int si468x_radio_pretune(struct si468x_radio *radio,
				enum si468x_func func)
{
	int retval;

	struct si468x_tune_freq_args args = {
		.injside	= SI468X_INJSIDE_AUTO,
		.antcap		= 0,
		.direct_tune	= SI468X_SELECT_MAIN_PROGRAM_SERVICE,
		.program_id	= 0,
	};

	if (radio->core->si468x_device_info->has_hd)
		args.tunemode	= SI468X_TUNEMODE_FAST_WITH_HD;
	else
		args.tunemode	= SI468X_TUNEMODE_FAST_NO_HD;

	switch (func) {
	case SI468X_FUNC_MINI_BOOT:
	case SI468X_FUNC_BOOTLOADER:
		retval = 0;
		break;
	case SI468X_FUNC_AM_RECEIVER:
		args.freq = v4l2_to_si468x(radio->core,
					   0.6 * FREQ_MUL);
		retval = radio->ops->tune_freq(radio->core, &args);
		break;
	case SI468X_FUNC_FM_RECEIVER:
		args.freq = v4l2_to_si468x(radio->core,
					   92 * FREQ_MUL);
		retval = radio->ops->tune_freq(radio->core, &args);
		break;
	case SI468X_FUNC_DAB_RECEIVER:
		retval = si468x_radio_dab_load_valid_frequencies(radio, &args);
		if (retval < 0)
			return retval;
		retval = si468x_radio_dab_get_ensembles(radio, &args);
		if (retval < 0)
			return retval;
		break;
	default:
		WARN(1, "Unexpected tuner function value\n");
		retval = -EINVAL;
	}

	return retval;
}

static int si468x_radio_do_post_powerup_init(struct si468x_radio *radio,
					     enum si468x_func func)
{
	int err;

	if ((func == SI468X_FUNC_MINI_BOOT) ||
	    (func == SI468X_FUNC_BOOTLOADER))
		return 0;

	regcache_mark_dirty(radio->core->regmap_common);
	regcache_cache_only(radio->core->regmap_common, false);
	err = regcache_sync(radio->core->regmap_common);
	if (err < 0)
		return err;
	switch (func) {
	case SI468X_FUNC_AM_RECEIVER:
		regcache_mark_dirty(radio->core->regmap_am);
		regcache_cache_only(radio->core->regmap_am, false);
		err = regcache_sync(radio->core->regmap_am);
		break;
	case SI468X_FUNC_FM_RECEIVER:
		regcache_mark_dirty(radio->core->regmap_fm);
		regcache_cache_only(radio->core->regmap_fm, false);
		err = regcache_sync(radio->core->regmap_fm);
		break;
	case SI468X_FUNC_DAB_RECEIVER:
		regcache_mark_dirty(radio->core->regmap_dab);
		regcache_cache_only(radio->core->regmap_dab, false);
		err = regcache_sync(radio->core->regmap_dab);
		break;
	default:
		err = -EINVAL;
	}
	if (err < 0)
		return err;

	err = si468x_radio_init_vtable(radio, func);
	if (err < 0)
		return err;

	return err;
}

static int si468x_radio_change_func(struct si468x_radio *radio,
				    enum si468x_func func)
{
	int err;
	/*
	 * Since power/up down is a very time consuming operation,
	 * try to avoid doing it if the requested mode matches the one
	 * the tuner is in
	 */
	if (func == radio->core->power_up_parameters.func)
		return 0;

	si468x_core_stop(radio->core);
	/*
	  Set the desired radio tuner function
	 */
	radio->core->power_up_parameters.func = func;

	err = si468x_core_start(radio->core);
	if (err < 0)
		return err;

	/*
	 * No need to do the rest of manipulations for the bootlader
	 * mode
	 */
	if (func != SI468X_FUNC_AM_RECEIVER &&
	    func != SI468X_FUNC_FM_RECEIVER &&
	    func != SI468X_FUNC_DAB_RECEIVER)
		return err;

	err = si468x_radio_do_post_powerup_init(radio, func);
	if (err < 0)
		return err;
	return si468x_radio_pretune(radio, func);
}

static int si468x_radio_g_frequency(struct file *file, void *priv,
			      struct v4l2_frequency *f)
{
	int err;
	struct si468x_radio *radio = video_drvdata(file);

	if (f->tuner != 0 ||
	    f->type  != V4L2_TUNER_RADIO)
		return -EINVAL;

	si468x_core_lock(radio->core);

	if (radio->ops->rsq_status) {
		struct si468x_rsq_status_report rsq_report;
		struct si468x_rsq_status_args   args = {
			.rsqack		= false,
			.digradack	= false,
			.attune		= true,
			.cancel		= false,
			.fiberrack	= false,
			.stcack		= false,
		};

		err = radio->ops->rsq_status(radio->core, &args, &rsq_report);
		if (err > 0)
			f->frequency = si468x_to_v4l2(radio->core,
						      rsq_report.readfreq);
	} else {
		err = -EINVAL;
	}

	si468x_core_unlock(radio->core);

	return (err < 0) ? err : 0;
}

static int si468x_radio_s_frequency(struct file *file, void *priv,
				    const struct v4l2_frequency *f)
{
	int err;
	u32 freq = f->frequency;
	struct si468x_tune_freq_args args;
	struct si468x_radio *radio = video_drvdata(file);
	enum si468x_func func;

	if (f->tuner != 0 ||
	    f->type  != V4L2_TUNER_RADIO)
		return -EINVAL;

	if (si468x_radio_freq_is_inside_of_the_band(freq, SI468X_BAND_AM))
		func = SI468X_FUNC_AM_RECEIVER;
	else if (si468x_radio_freq_is_inside_of_the_band(freq, SI468X_BAND_FM))
		func = SI468X_FUNC_FM_RECEIVER;
	else if (si468x_radio_freq_is_inside_of_the_band(freq, SI468X_BAND_DAB))
		func = SI468X_FUNC_DAB_RECEIVER;
	else
		return -EINVAL; 

	si468x_core_lock(radio->core);

	err = si468x_radio_change_func(radio, func);
	if (err < 0)
		goto unlock;

	args.injside		= SI468X_INJSIDE_AUTO;
	args.freq		= v4l2_to_si468x(radio->core, freq);
	args.antcap		= 0;
	args.direct_tune	= SI468X_SELECT_MAIN_PROGRAM_SERVICE;
	args.program_id		= 0;
	args.dab_freq_list	= loaded_dab_freq_list;
	if (radio->core->si468x_device_info->has_hd)
		args.tunemode	= SI468X_TUNEMODE_FAST_WITH_HD;
	else
		args.tunemode	= SI468X_TUNEMODE_FAST_NO_HD;

	err = radio->ops->tune_freq(radio->core, &args);
unlock:
	si468x_core_unlock(radio->core);
	return (err < 0) ? err : 0;
}

static int si468x_radio_s_hw_freq_seek(struct file *file, void *priv,
				       const struct v4l2_hw_freq_seek *seek)
{
	int err = 0;
	enum si468x_func func;
	u32 rangelow = seek->rangelow, rangehigh = seek->rangehigh;
	struct si468x_radio *radio = video_drvdata(file);
	struct si468x_tune_freq_args args = {
		.injside	= SI468X_INJSIDE_AUTO,
		.antcap		= 0,
	};

	if (file->f_flags & O_NONBLOCK)
		return -EAGAIN;

	if (seek->tuner != 0 ||
	    seek->type  != V4L2_TUNER_RADIO)
		return -EINVAL;

	si468x_core_lock(radio->core);

	if (!rangelow) {
		switch (radio->core->power_up_parameters.func) {
		case SI468X_FUNC_AM_RECEIVER:
			err = regmap_read(radio->core->regmap_am,
					  SI468X_PROP_AM_SEEK_BAND_BOTTOM,
					  &rangelow);
			if (err)
				goto unlock;
			rangelow = si468x_to_v4l2(radio->core, rangelow);
			break;
		default:
		case SI468X_FUNC_FM_RECEIVER:
			err = regmap_read(radio->core->regmap_fm,
					  SI468X_PROP_FM_SEEK_BAND_BOTTOM,
					  &rangelow);
			if (err)
				goto unlock;
			rangelow = si468x_to_v4l2(radio->core, rangelow);
			break;
		case SI468X_FUNC_DAB_RECEIVER:
			rangelow = si468x_bands[SI468X_BAND_DAB].rangelow;
			break;
		}
	}
	if (!rangehigh) {
		switch (radio->core->power_up_parameters.func) {
		case SI468X_FUNC_AM_RECEIVER:
			err = regmap_read(radio->core->regmap_am,
					  SI468X_PROP_AM_SEEK_BAND_TOP,
					  &rangehigh);
			if (err)
				goto unlock;
			rangehigh = si468x_to_v4l2(radio->core, rangehigh);
			break;
		default:
		case SI468X_FUNC_FM_RECEIVER:
			err = regmap_read(radio->core->regmap_fm,
					  SI468X_PROP_FM_SEEK_BAND_TOP,
					  &rangehigh);
			if (err)
				goto unlock;
			rangehigh = si468x_to_v4l2(radio->core, rangehigh);
			break;
		case SI468X_FUNC_DAB_RECEIVER:
			rangehigh = si468x_bands[SI468X_BAND_DAB].rangehigh;
			break;
		}
	}

	if (radio->core->power_up_parameters.func == SI468X_FUNC_AM_RECEIVER ||
	    radio->core->power_up_parameters.func == SI468X_FUNC_FM_RECEIVER) {
		if (rangelow > rangehigh) {
			err = -EINVAL;
			goto unlock;
		}
	}

	if (si468x_radio_range_is_inside_of_the_band(rangelow, rangehigh,
						     SI468X_BAND_FM)) {
		func = SI468X_FUNC_FM_RECEIVER;

	} else if (radio->core->si468x_device_info->has_am &&
		   si468x_radio_range_is_inside_of_the_band(rangelow, rangehigh,
							    SI468X_BAND_AM)) {
		func = SI468X_FUNC_AM_RECEIVER;
	} else if (radio->core->si468x_device_info->has_dab &&
		   si468x_radio_freq_is_inside_of_the_band(rangelow,
							   SI468X_BAND_DAB)) {
		func = SI468X_FUNC_DAB_RECEIVER;
	} else {
		err = -EINVAL;
		goto unlock;
	}

	err = si468x_radio_change_func(radio, func);
	if (err < 0)
		goto unlock;

	if (seek->rangehigh) {
		switch (func) {
		case SI468X_FUNC_AM_RECEIVER:
			err = regmap_write(radio->core->regmap_am,
					   SI468X_PROP_AM_SEEK_BAND_TOP,
					   v4l2_to_si468x(radio->core,
							  seek->rangehigh));
			break;
		case SI468X_FUNC_FM_RECEIVER:
			err = regmap_write(radio->core->regmap_fm,
					   SI468X_PROP_FM_SEEK_BAND_TOP,
					   v4l2_to_si468x(radio->core,
							  seek->rangehigh));
			break;
		default:
			break;
		}
		if (err)
			goto unlock;
	}
	if (seek->rangelow) {
		switch (func) {
		case SI468X_FUNC_AM_RECEIVER:
			err = regmap_write(radio->core->regmap_am,
					   SI468X_PROP_AM_SEEK_BAND_BOTTOM,
					   v4l2_to_si468x(radio->core,
							  seek->rangelow));
			break;
		case SI468X_FUNC_FM_RECEIVER:
			err = regmap_write(radio->core->regmap_fm,
					   SI468X_PROP_FM_SEEK_BAND_BOTTOM,
					   v4l2_to_si468x(radio->core,
							  seek->rangelow));
			break;
		default:
			break;
		}
		if (err)
			goto unlock;
	}
	if (seek->spacing) {
		switch (func) {
		case SI468X_FUNC_AM_RECEIVER:
			err = regmap_write(radio->core->regmap_am,
					     SI468X_PROP_AM_SEEK_FREQUENCY_SPACING,
					     v4l2_to_si468x(radio->core,
							    seek->spacing));
			break;
		case SI468X_FUNC_FM_RECEIVER:
			err = regmap_write(radio->core->regmap_fm,
					     SI468X_PROP_FM_SEEK_FREQUENCY_SPACING,
					     v4l2_to_si468x(radio->core,
							    seek->spacing));
			break;
		default:
			break;
		}
		if (err)
			goto unlock;
	}

	err = radio->ops->seek_start(radio->core, seek, &args);
unlock:
	si468x_core_unlock(radio->core);

	return err;
}

static int si468x_radio_g_volatile_ctrl(struct v4l2_ctrl *ctrl)
{
	int retval;
	struct si468x_radio *radio = v4l2_ctrl_handler_to_radio(ctrl->handler);
	retval = 0;

	si468x_core_lock(radio->core);

	switch (ctrl->id) {
	default:
		retval = -EINVAL;
		break;
	}
	si468x_core_unlock(radio->core);
	return retval;
}

static int si468x_radio_s_ctrl(struct v4l2_ctrl *ctrl)
{
	int retval = 0;
	struct si468x_radio *radio = v4l2_ctrl_handler_to_radio(ctrl->handler);

	si468x_core_lock(radio->core);

	switch (ctrl->id) {
	case V4L2_CID_AUDIO_MUTE:
		switch (radio->core->power_up_parameters.func) {
		case SI468X_FUNC_AM_RECEIVER:
		case SI468X_FUNC_FM_RECEIVER:
		case SI468X_FUNC_DAB_RECEIVER:
			if (ctrl->val)
				retval = regmap_write(radio->core->regmap_common,
					SI468X_PROP_AUDIO_MUTE, 3);
			else
				retval = regmap_write(radio->core->regmap_common,
					SI468X_PROP_AUDIO_MUTE, 0);
			break;
		default:
			retval = -EINVAL;
			break;
		}
		break;
	case V4L2_CID_AUDIO_VOLUME:
		switch (radio->core->power_up_parameters.func) {
		case SI468X_FUNC_AM_RECEIVER:
		case SI468X_FUNC_FM_RECEIVER:
		case SI468X_FUNC_DAB_RECEIVER:
			if (ctrl->val != 0)
				regmap_write(radio->core->regmap_common,
					SI468X_PROP_I2S_ENABLED,
					0x8001);
			else
				regmap_write(radio->core->regmap_common,
					SI468X_PROP_DAC_ENABLED,
					0x8002);
			retval = regmap_write(radio->core->regmap_common,
				SI468X_PROP_AUDIO_ANALOG_VOLUME,
				ctrl->val);
			break;
		default:
			retval = -EINVAL;
			break;
		}
		break;
	case V4L2_CID_TUNE_DEEMPHASIS:
		retval = regmap_write(radio->core->regmap_fm,
				      SI468X_PROP_FM_AUDIO_DE_EMPHASIS,
				      ctrl->val);
		break;
	case V4L2_CID_SI468X_RSSI_THRESHOLD:
		switch (radio->core->power_up_parameters.func) {
		case SI468X_FUNC_AM_RECEIVER:
		retval = regmap_write(radio->core->regmap_am,
				      SI468X_PROP_AM_VALID_RSSI_THRESHOLD,
				      ctrl->val);
			break;
		case SI468X_FUNC_FM_RECEIVER:
		retval = regmap_write(radio->core->regmap_fm,
				      SI468X_PROP_FM_VALID_RSSI_THRESHOLD,
				      ctrl->val);
			break;
		case SI468X_FUNC_DAB_RECEIVER:
		retval = regmap_write(radio->core->regmap_dab,
				      SI468X_PROP_DAB_VALID_RSSI_THRESHOLD,
				      ctrl->val);
			break;
		default:
			break;
		}
		break;
	case V4L2_CID_SI468X_SNR_THRESHOLD:
		switch (radio->core->power_up_parameters.func) {
		case SI468X_FUNC_AM_RECEIVER:
			retval = regmap_write(radio->core->regmap_am,
					      SI468X_PROP_AM_VALID_SNR_THRESHOLD,
					      ctrl->val);
			break;
		case SI468X_FUNC_FM_RECEIVER:
			retval = regmap_write(radio->core->regmap_fm,
					      SI468X_PROP_FM_VALID_SNR_THRESHOLD,
					      ctrl->val);
			break;
		default:
			break;
		}
		break;
	case V4L2_CID_SI468X_MAX_TUNE_ERROR:
		switch (radio->core->power_up_parameters.func) {
		case SI468X_FUNC_AM_RECEIVER:
			retval = regmap_write(radio->core->regmap_am,
					      SI468X_PROP_AM_VALID_MAX_TUNE_ERROR,
					      ctrl->val);
			break;
		case SI468X_FUNC_FM_RECEIVER:
			retval = regmap_write(radio->core->regmap_fm,
					      SI468X_PROP_FM_VALID_MAX_TUNE_ERROR,
					      ctrl->val);
			break;
		default:
			break;
		}
		break;
	case V4L2_CID_RDS_RECEPTION:
		if (si468x_core_is_in_fm_receiver_mode(radio->core)) {
			if (ctrl->val) {
				retval = regmap_write(radio->core->regmap_fm,
						      SI468X_PROP_FM_RDS_INTERRUPT_FIFO_COUNT,
						      radio->core->rds_fifo_depth);
				if (retval < 0)
					break;

				retval = regmap_write(radio->core->regmap_fm,
						      SI468X_PROP_FM_RDS_INTERRUPT_SOURCE,
						      SI468X_RDSRECV);
				if (retval < 0)
					break;

				/* Drain RDS FIFO before enabling RDS processing */
				retval = si468x_core_cmd_fm_rds_status(radio->core,
								       false,
								       true,
								       true,
								       NULL);
				if (retval < 0)
					break;

				retval = regmap_update_bits(radio->core->regmap_fm,
							    SI468X_PROP_FM_RDS_CONFIG,
							    SI468X_PROP_RDSEN_MASK,
							    SI468X_PROP_RDSEN);
			} else {
				retval = regmap_update_bits(radio->core->regmap_fm,
							    SI468X_PROP_FM_RDS_CONFIG,
							    SI468X_PROP_RDSEN_MASK,
							    !SI468X_PROP_RDSEN);
			}
		}
		break;
	default:
		retval = -EINVAL;
		break;
	}

	si468x_core_unlock(radio->core);

	return retval;
}

#ifdef CONFIG_VIDEO_ADV_DEBUG
static int si468x_radio_g_register(struct file *file, void *fh,
				   struct v4l2_dbg_register *reg)
{
	int err;
	unsigned int value;
	struct si468x_radio *radio = video_drvdata(file);

	si468x_core_lock(radio->core);
	reg->size = 2;
	err = regmap_read(radio->core->regmap_common,
			  (unsigned int)reg->reg, &value);
	reg->val = value;
	si468x_core_unlock(radio->core);

	return err;
}
static int si468x_radio_s_register(struct file *file, void *fh,
				   const struct v4l2_dbg_register *reg)
{

	int err;
	struct si468x_radio *radio = video_drvdata(file);

	si468x_core_lock(radio->core);
	err = regmap_write(radio->core->regmap_common,
			   (unsigned int)reg->reg,
			   (unsigned int)reg->val);
	si468x_core_unlock(radio->core);

	return err;
}
#endif

/**
 * si468x_radio_load_firmware_async()
 * @work: struct work_struct being passed to the function by the
 * kernel.
 *
 * boots the device and loads the propper firmware
 */
void si468x_radio_load_firmware_async(struct work_struct *work)
{
	int err;

	struct si468x_radio *radio = container_of(work, struct si468x_radio,
						load_firmware_async);

	si468x_core_lock(radio->core);
	err = si473x_core_set_power_state(radio->core,
					  SI468X_STATE_POWER_UP);
	if (err < 0)
		goto done;

	err = si468x_radio_do_post_powerup_init(radio,
						radio->core->power_up_parameters.func);
	if (err < 0)
		goto power_down;

	err = si468x_radio_pretune(radio,
				   radio->core->power_up_parameters.func);
	if (err < 0)
		goto power_down;

	si468x_core_unlock(radio->core);

	return;
power_down:
	si473x_core_set_power_state(radio->core,
				    SI468X_STATE_POWER_DOWN);
done:
	si468x_core_unlock(radio->core);
}

static int si468x_radio_fops_open(struct file *file)
{
	struct si468x_radio *radio = video_drvdata(file);
	int err;

	err = v4l2_fh_open(file);
	if (err)
		return err;

	if (v4l2_fh_is_singular_file(file)) {
		schedule_work(&radio->load_firmware_async);
		v4l2_ctrl_handler_setup(&radio->ctrl_handler);
	}

	return err;
}

static int si468x_radio_fops_release(struct file *file)
{
	int err;
	struct si468x_radio *radio = video_drvdata(file);

	if (v4l2_fh_is_singular_file(file) &&
	    atomic_read(&radio->core->is_alive))
		si473x_core_set_power_state(radio->core,
					    SI468X_STATE_POWER_DOWN);

	err = v4l2_fh_release(file);

	/* boot to FM next time (probing is faster with mini patch) */
	if ((radio->core->power_up_parameters.func == SI468X_FUNC_MINI_BOOT) ||
	    (radio->core->power_up_parameters.func == SI468X_FUNC_BOOTLOADER))
		radio->core->power_up_parameters.func = SI468X_FUNC_FM_RECEIVER;

	return err;
}

static ssize_t si468x_radio_fops_read(struct file *file, char __user *buf,
				      size_t count, loff_t *ppos)
{
	ssize_t      rval;
	size_t       fifo_len;
	unsigned int copied;

	struct si468x_radio *radio = video_drvdata(file);

	/* block if no new data available */
	if (kfifo_is_empty(&radio->core->rds_fifo)) {
		if (file->f_flags & O_NONBLOCK)
			return -EWOULDBLOCK;

		rval = wait_event_interruptible(radio->core->rds_read_queue,
						(!kfifo_is_empty(&radio->core->rds_fifo) ||
						 !atomic_read(&radio->core->is_alive)));
		if (rval < 0)
			return -EINTR;

		if (!atomic_read(&radio->core->is_alive))
			return -ENODEV;
	}

	fifo_len = kfifo_len(&radio->core->rds_fifo);

	if (kfifo_to_user(&radio->core->rds_fifo, buf,
			  min(fifo_len, count),
			  &copied) != 0) {
		dev_warn(&radio->videodev.dev,
			 "Error during FIFO to userspace copy\n");
		rval = -EIO;
	} else {
		rval = (ssize_t)copied;
	}

	return rval;
}

static __poll_t si468x_radio_fops_poll(struct file *file,
				struct poll_table_struct *pts)
{
	struct si468x_radio *radio = video_drvdata(file);
	__poll_t req_events = poll_requested_events(pts);
	__poll_t err = v4l2_ctrl_poll(file, pts);

	if (req_events & (EPOLLIN | EPOLLRDNORM)) {
		if (atomic_read(&radio->core->is_alive))
			poll_wait(file, &radio->core->rds_read_queue, pts);

		if (!atomic_read(&radio->core->is_alive))
			err = EPOLLHUP;

		if (!kfifo_is_empty(&radio->core->rds_fifo))
			err = EPOLLIN | EPOLLRDNORM;
	}

	return err;
}

static const struct v4l2_file_operations si468x_fops = {
	.owner			= THIS_MODULE,
	.read			= si468x_radio_fops_read,
	.poll			= si468x_radio_fops_poll,
	.unlocked_ioctl		= video_ioctl2,
	.open			= si468x_radio_fops_open,
	.release		= si468x_radio_fops_release,
};


static const struct v4l2_ioctl_ops si4761_ioctl_ops = {
	.vidioc_querycap		= si468x_radio_querycap,
	.vidioc_g_tuner			= si468x_radio_g_tuner,
	.vidioc_s_tuner			= si468x_radio_s_tuner,

	.vidioc_g_frequency		= si468x_radio_g_frequency,
	.vidioc_s_frequency		= si468x_radio_s_frequency,
	.vidioc_s_hw_freq_seek		= si468x_radio_s_hw_freq_seek,
	.vidioc_enum_freq_bands		= si468x_radio_enum_freq_bands,

	.vidioc_subscribe_event		= v4l2_ctrl_subscribe_event,
	.vidioc_unsubscribe_event	= v4l2_event_unsubscribe,

#ifdef CONFIG_VIDEO_ADV_DEBUG
	.vidioc_g_register		= si468x_radio_g_register,
	.vidioc_s_register		= si468x_radio_s_register,
#endif
};

static const struct video_device si468x_viddev_template = {
	.fops			= &si468x_fops,
	.name			= DRIVER_NAME,
	.release		= video_device_release_empty,
};

static ssize_t si468x_radio_read_acf_blob(struct file *file,
					  char __user *user_buf,
					  size_t count, loff_t *ppos)
{
	int err;
	struct si468x_radio *radio = file->private_data;
	struct si468x_acf_status_report acf_report;

	si468x_core_lock(radio->core);
	if (radio->ops->acf_status)
		err = radio->ops->acf_status(radio->core, &acf_report);
	else
		err = -ENOENT;
	si468x_core_unlock(radio->core);

	if (err < 0)
		return err;

	return simple_read_from_buffer(user_buf, count, ppos, &acf_report,
				       sizeof(acf_report));
}

static const struct file_operations radio_acf_fops = {
	.open	= simple_open,
	.llseek = default_llseek,
	.read	= si468x_radio_read_acf_blob,
};

static ssize_t si468x_radio_read_rds_blckcnt_blob(struct file *file,
						  char __user *user_buf,
						  size_t count, loff_t *ppos)
{
	int err;
	struct si468x_radio *radio = file->private_data;
	struct si468x_rds_blockcount_report rds_report;

	si468x_core_lock(radio->core);
	if (radio->ops->rds_blckcnt)
		err = radio->ops->rds_blckcnt(radio->core, true,
					       &rds_report);
	else
		err = -ENOENT;
	si468x_core_unlock(radio->core);

	if (err < 0)
		return err;

	return simple_read_from_buffer(user_buf, count, ppos, &rds_report,
				       sizeof(rds_report));
}

static const struct file_operations radio_rds_blckcnt_fops = {
	.open	= simple_open,
	.llseek = default_llseek,
	.read	= si468x_radio_read_rds_blckcnt_blob,
};

static ssize_t si468x_radio_read_agc_blob(struct file *file,
					  char __user *user_buf,
					  size_t count, loff_t *ppos)
{
	int err;
	struct si468x_radio *radio = file->private_data;
	struct si468x_agc_status_report agc_report;

	si468x_core_lock(radio->core);
	if (radio->ops->rds_blckcnt)
		err = radio->ops->agc_status(radio->core, &agc_report);
	else
		err = -ENOENT;
	si468x_core_unlock(radio->core);

	if (err < 0)
		return err;

	return simple_read_from_buffer(user_buf, count, ppos, &agc_report,
				       sizeof(agc_report));
}

static const struct file_operations radio_agc_fops = {
	.open	= simple_open,
	.llseek = default_llseek,
	.read	= si468x_radio_read_agc_blob,
};

static ssize_t si468x_radio_read_rsq_blob(struct file *file,
					  char __user *user_buf,
					  size_t count, loff_t *ppos)
{
	int err;
	struct si468x_radio *radio = file->private_data;
	struct si468x_rsq_status_report rsq_report;
	struct si468x_rsq_status_args args = {
		.rsqack		= false,
		.digradack	= false,
		.attune		= false,
		.cancel		= false,
		.fiberrack	= false,
		.stcack		= false,
	};

	si468x_core_lock(radio->core);
	if (radio->ops->rds_blckcnt)
		err = radio->ops->rsq_status(radio->core, &args, &rsq_report);
	else
		err = -ENOENT;
	si468x_core_unlock(radio->core);

	if (err < 0)
		return err;

	return simple_read_from_buffer(user_buf, count, ppos, &rsq_report,
				       sizeof(rsq_report));
}

static const struct file_operations radio_rsq_fops = {
	.open	= simple_open,
	.llseek = default_llseek,
	.read	= si468x_radio_read_rsq_blob,
};

static int si468x_radio_init_debugfs(struct si468x_radio *radio)
{
	struct dentry	*dentry;
	int		ret;

	dentry = debugfs_create_dir(dev_name(radio->v4l2dev.dev), NULL);
	if (IS_ERR(dentry)) {
		ret = PTR_ERR(dentry);
		goto exit;
	}
	radio->debugfs = dentry;

	dentry = debugfs_create_file("acf", S_IRUGO,
				     radio->debugfs, radio, &radio_acf_fops);
	if (IS_ERR(dentry)) {
		ret = PTR_ERR(dentry);
		goto cleanup;
	}

	dentry = debugfs_create_file("rds_blckcnt", S_IRUGO,
				     radio->debugfs, radio,
				     &radio_rds_blckcnt_fops);
	if (IS_ERR(dentry)) {
		ret = PTR_ERR(dentry);
		goto cleanup;
	}

	dentry = debugfs_create_file("agc", S_IRUGO,
				     radio->debugfs, radio, &radio_agc_fops);
	if (IS_ERR(dentry)) {
		ret = PTR_ERR(dentry);
		goto cleanup;
	}

	dentry = debugfs_create_file("rsq", S_IRUGO,
				     radio->debugfs, radio, &radio_rsq_fops);
	if (IS_ERR(dentry)) {
		ret = PTR_ERR(dentry);
		goto cleanup;
	}

	return 0;
cleanup:
	debugfs_remove_recursive(radio->debugfs);
exit:
	return ret;
}

static int si468x_radio_add_new_custom(struct si468x_radio *radio,
				       enum si468x_ctrl_idx idx)
{
	int rval;
	struct v4l2_ctrl *ctrl;

	ctrl = v4l2_ctrl_new_custom(&radio->ctrl_handler,
				    &si468x_ctrls[idx],
				    NULL);
	rval = radio->ctrl_handler.error;
	if (ctrl == NULL && rval)
		dev_err(radio->v4l2dev.dev,
			"Could not initialize '%s' control %d\n",
			si468x_ctrls[idx].name, rval);

	return rval;
}

static int si468x_radio_probe(struct platform_device *pdev)
{
	int rval;
	struct si468x_radio *radio;
	struct v4l2_ctrl *ctrl;

	static atomic_t instance = ATOMIC_INIT(0);

	radio = devm_kzalloc(&pdev->dev, sizeof(*radio), GFP_KERNEL);
	if (!radio)
		return -ENOMEM;

	radio->core = dev_get_drvdata(pdev->dev.parent);
	if (!radio->core) {
		dev_err(&pdev->dev, "failed to get drvdata.\n");
		return -ENOMEM;
	}

	v4l2_device_set_name(&radio->v4l2dev, DRIVER_NAME, &instance);

	rval = v4l2_device_register(&pdev->dev, &radio->v4l2dev);
	if (rval) {
		dev_err(&pdev->dev, "Cannot register v4l2_device.\n");
		return rval;
	}

	memcpy(&radio->videodev, &si468x_viddev_template,
	       sizeof(struct video_device));

	radio->videodev.v4l2_dev  = &radio->v4l2dev;
	radio->videodev.ioctl_ops = &si4761_ioctl_ops;
	radio->videodev.device_caps = V4L2_CAP_TUNER | V4L2_CAP_RADIO |
				      V4L2_CAP_HW_FREQ_SEEK;

	si468x_core_lock(radio->core);
	radio->videodev.device_caps |= V4L2_CAP_RDS_CAPTURE |
				       V4L2_CAP_READWRITE;
	si468x_core_unlock(radio->core);

	video_set_drvdata(&radio->videodev, radio);
	platform_set_drvdata(pdev, radio);

	INIT_WORK(&radio->load_firmware_async, si468x_radio_load_firmware_async);
	radio->core->loaded_dab_freq_list = loaded_dab_freq_list;

	radio->v4l2dev.ctrl_handler = &radio->ctrl_handler;
	v4l2_ctrl_handler_init(&radio->ctrl_handler,
			       1 + ARRAY_SIZE(si468x_ctrls));

	rval = si468x_radio_add_new_custom(radio, SI468X_IDX_RSSI_THRESHOLD);
	if (rval < 0)
		goto exit;

	rval = si468x_radio_add_new_custom(radio, SI468X_IDX_SNR_THRESHOLD);
	if (rval < 0)
		goto exit;

	rval = si468x_radio_add_new_custom(radio, SI468X_IDX_MAX_TUNE_ERROR);
	if (rval < 0)
		goto exit;

	ctrl = v4l2_ctrl_new_std_menu(&radio->ctrl_handler,
				      &si468x_ctrl_ops,
				      V4L2_CID_TUNE_DEEMPHASIS,
				      V4L2_DEEMPHASIS_50_uS, 0, 0);
	rval = radio->ctrl_handler.error;
	if (ctrl == NULL && rval) {
		dev_err(&pdev->dev, "Could not initialize V4L2_CID_TUNE_DEEMPHASIS control %d\n",
			rval);
		goto exit;
	}

	ctrl = v4l2_ctrl_new_std(&radio->ctrl_handler,
				 &si468x_ctrl_ops,
				 V4L2_CID_AUDIO_MUTE,
				 0, 3, 1, 0);
	rval = radio->ctrl_handler.error;
	if (ctrl == NULL && rval) {
		dev_err(&pdev->dev, "Could not initialize V4L2_CID_AUDIO_MUTE control %d\n",
			rval);
		goto exit;
	}

	ctrl = v4l2_ctrl_new_std(&radio->ctrl_handler, &si468x_ctrl_ops,
				 V4L2_CID_AUDIO_VOLUME,
				 0, 0x3f, 1, 0);
	rval = radio->ctrl_handler.error;
	if (ctrl == NULL && rval) {
		dev_err(&pdev->dev, "Could not initialize V4L2_CID_AUDIO_VOLUME control %d\n",
			rval);
		goto exit;
	}

	ctrl = v4l2_ctrl_new_std(&radio->ctrl_handler, &si468x_ctrl_ops,
				 V4L2_CID_RDS_RECEPTION,
				 0, 1, 1, 1);
	rval = radio->ctrl_handler.error;
	if (ctrl == NULL && rval) {
		dev_err(&pdev->dev, "Could not initialize V4L2_CID_RDS_RECEPTION control %d\n",
			rval);
		goto exit;
	}

	/* register video device */
	rval = video_register_device(&radio->videodev, VFL_TYPE_RADIO, -1);
	if (rval < 0) {
		dev_err(&pdev->dev, "Could not register video device\n");
		goto exit;
	}

	rval = si468x_radio_init_debugfs(radio);
	if (rval < 0) {
		dev_err(&pdev->dev, "Could not create debugfs interface\n");
		goto exit;
	}

	return 0;
exit:
	v4l2_ctrl_handler_free(radio->videodev.ctrl_handler);
	return rval;
}

static int si468x_radio_remove(struct platform_device *pdev)
{
	struct si468x_radio *radio = platform_get_drvdata(pdev);

	v4l2_ctrl_handler_free(radio->videodev.ctrl_handler);
	video_unregister_device(&radio->videodev);
	v4l2_device_unregister(&radio->v4l2dev);
	debugfs_remove_recursive(radio->debugfs);

	return 0;
}

MODULE_ALIAS("platform:si468x-radio");

static struct platform_driver si468x_radio_driver = {
	.driver		= {
		.name	= DRIVER_NAME,
	},
	.probe		= si468x_radio_probe,
	.remove		= si468x_radio_remove,
};
module_platform_driver(si468x_radio_driver);

MODULE_AUTHOR("rpi Receiver <rpi-receiver@htl-steyr.ac.at>");
MODULE_DESCRIPTION("Driver for Si468x AM/FM/DAB Radio MFD Cell");
MODULE_LICENSE("GPL");
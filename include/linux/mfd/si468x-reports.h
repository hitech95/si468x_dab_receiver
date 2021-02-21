/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * include/media/si468x-platform.h -- Definitions of the data formats
 * returned by debugfs hooks
 *
 * Copyright (C) 2020 HTL Steyr - Austria
 * Copyright (C) 2020 Franz Parzer
 * based on the si476x driver of 
 * Copyright (C) 2013 Andrey Smirnov
 *
 * Author: Franz Parzer <rpi-receiver@htl-steyr.ac.at>
 * Author: Andrey Smirnov <andrew.smirnov@gmail.com>
 */

#ifndef __SI468X_REPORTS_H__
#define __SI468X_REPORTS_H__

/**
 * struct si468x_rsq_status - structure containing received signal
 * quality
 */
struct si468x_rsq_status_report {
	__u8  hdlevelhint, hdlevellint;
	__u8  snrhint, snrlint;
	__u8  rssihint, rssilint;
	__u8  bltf;
	__u8  hddetected;
	__u8  flt_hddetected;
	__u8  afcrl;
	__u8  valid;
	__u32 readfreq;
	__s8  freqoff;
	__s8  rssi;
	__s8  snr;
	__s8  mult;
	__u16 readantcap;
	__s8  hdlevel;
	__s8  flt_hdlevel;
	__s8  mod;
	__u8  ficerrint;
	__u8  acqint;
	__u8  ficerr;
	__u8  acq;
	__u8  fic_quality;
	__u8  cnr;
	__u16 fib_error_count;
	__u8  tune_index;
	__u8  fft_offset;
	__u16 cu_level;
	__u8  fast_dect;
} __packed;

/**
 * si468x_acf_status_report - ACF
 * (automatically controlled features) report results
 */
struct si468x_acf_status_report {
	__u8  blend_int;
	__u8  hicut_int;
	__u8  softmute_int;
	__u8  blend_conv;
	__u8  hicut_conv;
	__u8  softmute_conv;
	__u8  blend_state;
	__u8  hicut_state;
	__u8  softmute_state;
	__u8  smattn;
	__u8  hicut;
	__u8  pilot;
	__u8  stblend;
	__u8  lowcut;
	__u8  rfu1;
	__u8  rfu2;
	__u16 audio_level;
	__u16 cmft_noise_level;
} __packed;

/**
 * si468x_agc_status_report
 */
struct si468x_agc_status_report {
	__u8 vhflna;
	__u8 vhfcatt;
	__u8 vhfrattlo;
	__u8 vhfratthi;
	__u8 lmhfrtanklo;
	__u8 lmhfrtankhi;
	__u8 rfindex;
	__u8 lmhfcatt;
} __packed;

/**
 * si468x_rds_blockcount_report
 */
struct si468x_rds_blockcount_report {
	__u16 expected;
	__u16 received;
	__u16 uncorrectable;
} __packed;

#endif  /* __SI468X_REPORTS_H__ */

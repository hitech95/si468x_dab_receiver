// SPDX-License-Identifier: GPL-2.0-only
/*
 * drivers/mfd/si468x-cmd.c -- Subroutines implementing command
 * protocol of si468x series of chips
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

#ifndef SI468X_CMD_PRIV_H
#define SI468X_CMD_PRIV_H

#define SI468X_FIRMWARE_AMHD       "si468x/amhd_radio_3_0_6.bif"
#define SI468X_FIRMWARE_FMHD       "si468x/fmhd_radio_5_1_0.bin"
#define SI468X_FIRMWARE_DAB        "si468x/dab_radio_5_0_8.bin"
#define SI468X_FIRMWARE_PATCH_MINI "si468x/rom00_patch_mini.bin"
#define SI468X_FIRMWARE_PATCH      "si468x/rom00_patch.016.bin"

#define msb(x)                  ((u8)((u16) x >> 8))
#define lsb(x)                  ((u8)((u16) x &  0x00FF))

/* Returns the status byte and data for the last command sent to the device. */
#define CMD_RD_REPLY					0x00
#define CMD_RD_REPLY_NARGS				0
#define CMD_RD_REPLY_NRESP				6

/* Power-up the device and set system settings. */
#define CMD_POWER_UP					0x01
#define CMD_POWER_UP_NARGS				15
#define CMD_POWER_UP_NRESP				4

/* Loads an image from HOST over command interface */
#define CMD_HOST_LOAD					0x04
#define CMD_HOST_LOAD_NARGS				3
#define CMD_HOST_LOAD_NRESP				4

/* Loads an image from external FLASH over secondary SPI bus */
#define CMD_FLASH_LOAD					0x05
#define CMD_FLASH_LOAD_NARGS				11
#define CMD_FLASH_LOAD_NRESP				4

/* Prepares the bootloader to receive a new image. */
#define CMD_LOAD_INIT					0x06
#define CMD_LOAD_INIT_NARGS				1
#define CMD_LOAD_INIT_NRESP				4

/* Boots the image currently loaded in RAM. */
#define CMD_BOOT					0x07
#define CMD_BOOT_NARGS					1
#define CMD_BOOT_NRESP					4

/* Reports basic information about the device. */
#define CMD_GET_PART_INFO				0x08
#define CMD_GET_PART_INFO_NARGS				1
#define CMD_GET_PART_INFO_NRESP				23

/* Reports system state information. */
#define CMD_GET_SYS_STATE				0x09
#define CMD_GET_SYS_STATE_NARGS				1
#define CMD_GET_SYS_STATE_NRESP				6

/* Reports basic information about the device such as arguments used during POWER_UP. */
#define CMD_GET_POWER_UP_ARGS				0x0A
#define CMD_GET_POWER_UP_ARGS_NARGS			1
#define CMD_GET_POWER_UP_ARGS_NRESP			18

/* Reads a portion of response buffer from an offset. */
#define CMD_READ_OFFSET					0x10
#define CMD_READ_OFFSET_NARGS				3
#define CMD_READ_OFFSET_NRESP				5

/* Returns the Function revision information of the device. */
#define CMD_GET_FUNC_INFO				0x12
#define CMD_GET_FUNC_INFO_NARGS				1
#define CMD_GET_FUNC_INFO_NRESP				12

/* Sets the value of a property. */
#define CMD_SET_PROPERTY				0x13
#define CMD_SET_PROPERTY_NARGS				5
#define CMD_SET_PROPERTY_NRESP				4

/* Retrieve the value of a property. */
#define CMD_GET_PROPERTY				0x14
#define CMD_GET_PROPERTY_NARGS				3
#define CMD_GET_PROPERTY_NRESP				6

/* Writes data to the on board storage area at a specified offset. */
#define CMD_WRITE_STORAGE				0x15
#define CMD_WRITE_STORAGE_NARGS				8
#define CMD_WRITE_STORAGE_NRESP				4

/* Reads data from the on board storage area from a specified offset. */
#define CMD_READ_STORAGE				0x16
#define CMD_READ_STORAGE_NARGS				3
#define CMD_READ_STORAGE_NRESP				5

/* Reports the status of the AGC. */
#define CMD_GET_AGC_STATUS				0x17
#define CMD_GET_AGC_STATUS_NARGS			1
#define CMD_GET_AGC_STATUS_NRESP			23

/* Tunes the FM receiver to a frequency in 10 kHz steps. */
#define CMD_FM_TUNE_FREQ				0x30
#define CMD_FM_TUNE_FREQ_NARGS				6
#define CMD_FM_TUNE_FREQ_NRESP				4

/* Initiates a seek for a channel that meets the validation criteria for FM. */
#define CMD_FM_SEEK_START				0x31
#define CMD_FM_SEEK_START_NARGS				5
#define CMD_FM_SEEK_START_NRESP				4

/* Returns status information about the received signal quality. */
#define CMD_FM_RSQ_STATUS				0x32
#define CMD_FM_RSQ_STATUS_NARGS				1
#define CMD_FM_RSQ_STATUS_NRESP				22

/* Returns status information about automatically controlled features. */
#define CMD_FM_ACF_STATUS				0x33
#define CMD_FM_ACF_STATUS_NARGS				1
#define CMD_FM_ACF_STATUS_NRESP				11

/* Queries the status of RDS decoder and Fifo. */
#define CMD_FM_RDS_STATUS				0x34
#define CMD_FM_RDS_STATUS_NARGS				1
#define CMD_FM_RDS_STATUS_NRESP				20

/* Queries the block statistic info of RDS decoder. */
#define CMD_FM_RDS_BLOCKCOUNT				0x35
#define CMD_FM_RDS_BLOCKCOUNT_NARGS			1
#define CMD_FM_RDS_BLOCKCOUNT_NRESP			10

/* Tunes the AM receiver to a frequency in 1 kHz steps. */
#define CMD_AM_TUNE_FREQ				0x40
#define CMD_AM_TUNE_FREQ_NARGS				5
#define CMD_AM_TUNE_FREQ_NRESP				4

/* Initiates a seek for a channel that meets the validation criteria for AM. */
#define CMD_AM_SEEK_START				0x41
#define CMD_AM_SEEK_START_NARGS				5
#define CMD_AM_SEEK_START_NRESP				4

/* Returns status information about the received signal quality. */
#define CMD_AM_RSQ_STATUS				0x42
#define CMD_AM_RSQ_STATUS_NARGS				1
#define CMD_AM_RSQ_STATUS_NRESP				17

/* Returns status information about automatically controlled features. */
#define CMD_AM_ACF_STATUS				0x43
#define CMD_AM_ACF_STATUS_NARGS				1
#define CMD_AM_ACF_STATUS_NRESP				9

/* Gets a service list of the ensemble. */
#define CMD_GET_DIGITAL_SERVICE_LIST			0x80
#define CMD_GET_DIGITAL_SERVICE_LIST_NARGS		1
#define CMD_GET_DIGITAL_SERVICE_LIST_NRESP		6

/* Starts an audio or data service. */
#define CMD_START_DIGITAL_SERVICE			0x81
#define CMD_START_DIGITAL_SERVICE_NARGS			11
#define CMD_START_DIGITAL_SERVICE_NRESP			4

/* Stops an audio or data service. */
#define CMD_STOP_DIGITAL_SERVICE			0x82
#define CMD_STOP_DIGITAL_SERVICE_NARGS			11
#define CMD_STOP_DIGITAL_SERVICE_NRESP			4

/* Gets a block of data associated with one of the enabled data components of a digital services. */
#define CMD_GET_DIGITAL_SERVICE_DATA			0x84
#define CMD_GET_DIGITAL_SERVICE_DATA_NARGS		1
#define CMD_GET_DIGITAL_SERVICE_DATA_NRESP		24

/* Returns status information about the digital radio and ensemble. */
#define CMD_HD_DIGRAD_STATUS				0x92
#define CMD_HD_DIGRAD_STATUS_NARGS			1
#define CMD_HD_DIGRAD_STATUS_NRESP			23

/* Gets information about the various events related to the HD services. */
#define CMD_HD_GET_EVENT_STATUS				0x93
#define CMD_HD_GET_EVENT_STATUS_NARGS			1
#define CMD_HD_GET_EVENT_STATUS_NRESP			18

/* Retrieves information about the ensemble broadcaster. */
#define CMD_HD_GET_STATION_INFO				0x94
#define CMD_HD_GET_STATION_INFO_NARGS			1
#define CMD_HD_GET_STATION_INFO_NRESP			7

/* Retrieves PSD information. */
#define CMD_HD_GET_PSD_DECODE				0x95
#define CMD_HD_GET_PSD_DECODE_NARGS			2
#define CMD_HD_GET_PSD_DECODE_NRESP			9

/* Retrieves the HD Alert message. */
#define CMD_HD_GET_ALERT_MSG				0x96
#define CMD_HD_GET_ALERT_MSG_NARGS			1
#define CMD_HD_GET_ALERT_MSG_NRESP			4

/* Plays the HD Alert Tone. */
#define CMD_HD_PLAY_ALERT_TONE				0x97
#define CMD_HD_PLAY_ALERT_TONE_NARGS			1
#define CMD_HD_PLAY_ALERT_TONE_NRESP			4

/* Reads the current BER information. */
#define CMD_HD_TEST_GET_BER_INFO			0x98
#define CMD_HD_TEST_GET_BER_INFO_NARGS			1
#define CMD_HD_TEST_GET_BER_INFO_NRESP			44

/* Sets default ports retrieved after acquisition. */
#define CMD_HD_SET_ENABLED_PORTS			0x99
#define CMD_HD_SET_ENABLED_PORTS_NARGS			3
#define CMD_HD_SET_ENABLED_PORTS_NRESP			4

/* Gets default ports retrieved after acquisition. */
#define CMD_HD_GET_ENABLED_PORTS			0x9A
#define CMD_HD_GET_ENABLED_PORTS_NARGS			1
#define CMD_HD_GET_ENABLED_PORTS_NRESP			8

/* This command Returns status information about HD automatically controlled features. */
#define CMD_HD_ACF_STATUS				0x9C
#define CMD_HD_ACF_STATUS_NARGS				1
#define CMD_HD_ACF_STATUS_NRESP				10

/* Tunes the DAB Receiver to tune to a frequency between 168.16 and 239.20 MHz defined by the frequency table through DAB_SET_FREQ_LIST. */
#define CMD_DAB_TUNE_FREQ				0xB0
#define CMD_DAB_TUNE_FREQ_NARGS				5
#define CMD_DAB_TUNE_FREQ_NRESP				4

/* Returns status information about the digital radio and ensemble. */
#define CMD_DAB_DIGRAD_STATUS				0xB2
#define CMD_DAB_DIGRAD_STATUS_NARGS			1
#define CMD_DAB_DIGRAD_STATUS_NRESP			23

/* Gets information about the various events related to the DAB radio. */
#define CMD_DAB_GET_EVENT_STATUS			0xB3
#define CMD_DAB_GET_EVENT_STATUS_NARGS			1
#define CMD_DAB_GET_EVENT_STATUS_NRESP			8

/* Gets information about the current ensemble */
#define CMD_DAB_GET_ENSEMBLE_INFO			0xB4
#define CMD_DAB_GET_ENSEMBLE_INFO_NARGS			1
#define CMD_DAB_GET_ENSEMBLE_INFO_NRESP			26

/* gets the announcement support information. */
#define CMD_DAB_GET_ANNOUNCEMENT_SUPPORT_INFO		0xB5
#define CMD_DAB_GET_ANNOUNCEMENT_SUPPORT_INFO_NARGS	7
#define CMD_DAB_GET_ANNOUNCEMENT_SUPPORT_INFO_NRESP	12

/* gets announcement information from the announcement queue. */
#define CMD_DAB_GET_ANNOUNCEMENT_INFO			0xB6
#define CMD_DAB_GET_ANNOUNCEMENT_INFO_NARGS		0
#define CMD_DAB_GET_ANNOUNCEMENT_INFO_NRESP		16

/* Provides service linking (FIG 0/6) information for the passed in service ID. */
#define CMD_DAB_GET_SERVICE_LINKING_INFO		0xB7
#define CMD_DAB_GET_SERVICE_LINKING_INFO_NARGS		7
#define CMD_DAB_GET_SERVICE_LINKING_INFO_NRESP		16

/* Sets the DAB frequency table. The frequencies are in units of kHz. */
#define CMD_DAB_SET_FREQ_LIST				0xB8
#define CMD_DAB_SET_FREQ_LIST_NARGS			3
#define CMD_DAB_SET_FREQ_LIST_NRESP			4

/* Gets the DAB frequency table */
#define CMD_DAB_GET_FREQ_LIST				0xB9
#define CMD_DAB_GET_FREQ_LIST_NARGS			1
#define CMD_DAB_GET_FREQ_LIST_NRESP			8

/* Gets information about components within the ensemble if available. */
#define CMD_DAB_GET_COMPONENT_INFO			0xBB
#define CMD_DAB_GET_COMPONENT_INFO_NARGS		11
#define CMD_DAB_GET_COMPONENT_INFO_NRESP		33

/* Gets the ensemble time adjusted for the local time offset or the UTC. */
#define CMD_DAB_GET_TIME				0xBC
#define CMD_DAB_GET_TIME_NARGS				1
#define CMD_DAB_GET_TIME_NRESP				11

/* Gets audio service info */
#define CMD_DAB_GET_AUDIO_INFO				0xBD
#define CMD_DAB_GET_AUDIO_INFO_NARGS			1
#define CMD_DAB_GET_AUDIO_INFO_NRESP			20

/* Gets sub-channel info */
#define CMD_DAB_GET_SUBCHAN_INFO			0xBE
#define CMD_DAB_GET_SUBCHAN_INFO_NARGS			11
#define CMD_DAB_GET_SUBCHAN_INFO_NRESP			12

/* Gets ensemble freq info */
#define CMD_DAB_GET_FREQ_INFO				0xBF
#define CMD_DAB_GET_FREQ_INFO_NARGS			1
#define CMD_DAB_GET_FREQ_INFO_NRESP			20

/* Gets information about a service. */
#define CMD_DAB_GET_SERVICE_INFO			0xC0
#define CMD_DAB_GET_SERVICE_INFO_NARGS			7
#define CMD_DAB_GET_SERVICE_INFO_NRESP			26

/* Provides other ensemble (OE) services (FIG 0/24) information for the passed in service ID. */
#define CMD_DAB_GET_OE_SERVICES_INFO			0xC1
#define CMD_DAB_GET_OE_SERVICES_INFO_NARGS		7
#define CMD_DAB_GET_OE_SERVICES_INFO_NRESP		10

/* Returns status information about automatically controlled features. */
#define CMD_DAB_ACF_STATUS				0xC2
#define CMD_DAB_ACF_STATUS_NARGS			1
#define CMD_DAB_ACF_STATUS_NRESP			12

/* Returns the reported RSSI in 8.8 format. */
#define CMD_TEST_GET_RSSI				0xE5
#define CMD_TEST_GET_RSSI_NARGS				1
#define CMD_TEST_GET_RSSI_NRESP				6

/* Reads the current BER rate */
#define CMD_DAB_TEST_GET_BER_INFO			0xE8
#define CMD_DAB_TEST_GET_BER_INFO_NARGS			1
#define CMD_DAB_TEST_GET_BER_INFO_NRESP			12

#define CMD_MAX_ARGS_COUNT				(15)
#define CMD_MAX_RESP_COUNT				(44)

#define SI468X_DRIVER_RDS_FIFO_DEPTH	128
#define SI468X_SERVICE_DATA_MAX_LENGTH	0x10000

enum si468x_load_firmware_to {
	SI468X_LOAD_TO_HOST  = true,
	SI468X_LOAD_TO_FLASH = false,
};

const char *si468x_func_string_table[] = {
	[SI468X_FUNC_BOOTLOADER]   = "patch",
	[SI468X_FUNC_AM_RECEIVER]  = "am",
	[SI468X_FUNC_FM_RECEIVER]  = "fm",
	[SI468X_FUNC_DAB_RECEIVER] = "dab",
	[SI468X_FUNC_MINI_BOOT]    = "mini",
};

enum si468x_acf_irq_ack_bits {
	SI468X_ACF_ACK_INT	= BIT(0),
};

enum si468x_acf_status_report_bits {
	SI468X_ACF_SOFTMUTE_INT		= BIT(0),
	SI468X_ACF_HICUT_INT		= BIT(1),
	SI468X_ACF_BLEND_INT		= BIT(2),
	SI468X_ACF_SOFTMUTE_STATE	= BIT(0),
	SI468X_ACF_HICUT_STATE		= BIT(1),
	SI468X_ACF_BLEND_STATE		= BIT(2),
	SI468X_ACF_SOFTMUTE_CONV	= BIT(4),
	SI468X_ACF_HICUT_CONV		= BIT(5),
	SI468X_ACF_BLEND_CONV		= BIT(6),

	SI468X_ACF_SMATTN		= 0x1f,
	SI468X_ACF_PILOT		= BIT(7),
	SI468X_ACF_STBLEND		= ~SI468X_ACF_PILOT,
};

enum si468x_agc_status_report_bits {
	SI468X_AGC_MXHI			= BIT(5),
	SI468X_AGC_MXLO			= BIT(4),
	SI468X_AGC_LNAHI		= BIT(3),
	SI468X_AGC_LNALO		= BIT(2),
};

enum si468x_event_status_report_bits {
	SI468X_EVENT_RECFGINT		= BIT(7),
	SI468X_EVENT_RECFGWRNINT	= BIT(6),
	SI468X_EVENT_ANNOINT		= BIT(4),
	SI468X_EVENT_OESERVINT		= BIT(3),
	SI468X_EVENT_SERVLINKINT	= BIT(2),
	SI468X_EVENT_FREQINFOINT	= BIT(1),
	SI468X_EVENT_SVRLISTINT		= BIT(0),
	SI468X_EVENT_ANNO		= BIT(4),
	SI468X_EVENT_OESERV		= BIT(3),
	SI468X_EVENT_SERVLINK		= BIT(2),
	SI468X_EVENT_FREQ_INFO		= BIT(1),
	SI468X_EVENT_SVRLIST		= BIT(0),
};

enum si468x_errors {
	SI468X_ERR_UNSPECIFIED		= 0x01,
	SI468X_ERR_REPLY_OVERFLOW	= 0x02,
	SI468X_ERR_NOT_AVAILABLE	= 0x03,
	SI468X_ERR_NOT_SUPPORTED	= 0x04,
	SI468X_ERR_BAD_FREQUENCY	= 0x05,
	SI468X_ERR_COMMAND_NOT_FOUND	= 0x10,
	SI468X_ERR_BAD_ARG1		= 0x11,
	SI468X_ERR_BAD_ARG2		= 0x12,
	SI468X_ERR_BAD_ARG3		= 0x13,
	SI468X_ERR_BAD_ARG4		= 0x14,
	SI468X_ERR_BAD_ARG5		= 0x15,
	SI468X_ERR_BAD_ARG6		= 0x16,
	SI468X_ERR_BAD_ARG7		= 0x17,
	SI468X_ERR_COMMAND_BUSY		= 0x18,
	SI468X_ERR_AT_BAND_LIMIT	= 0x19,
	SI468X_ERR_BAD_NVM		= 0x20,
	SI468X_ERR_BAD_PATCH		= 0x30,
	SI468X_ERR_BAD_BOOT_MODE	= 0x31,
	SI468X_ERR_BAD_PROPERTY		= 0x40,
	SI468X_ERR_NOT_ACQUIRED		= 0x50,
	SI468X_ERR_APP_NOT_SUPPORTED	= 0xff,
};

struct si468x_digital_service_data_status_report {
	bool dsrvovflint, dsrvpcktint;
	u8   buff_count;
	u8   srv_state;
	u8   data_src;
	u8   dscty;
	u32  service_id, comp_id;
	u16  uatype;
	u16  byte_count;
	u16  seg_num, num_segs;
	u8   *payload;
};

static inline void si468x_core_start_rds_drainer_once(struct si468x_core *);
static inline void si468x_core_get_digital_service_list(struct si468x_core *);
static inline void si468x_core_get_digital_service_data(struct si468x_core *);

#endif /* __SI468X_CMD_PRIV_H__ */

/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * include/media/si468x-core.h -- Common definitions for si468x core
 * device
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

#ifndef SI468X_CORE_H
#define SI468X_CORE_H

#include <linux/kfifo.h>
#include <linux/regmap.h>
#include <linux/mfd/core.h>
#include <linux/of_device.h>
#include <linux/videodev2.h>
#include <linux/regulator/consumer.h>

#include <linux/mfd/si468x-platform.h>
#include <linux/mfd/si468x-reports.h>

#define SI468X_MAX_HOST_LOAD_BYTES 512
#define SI468X_DAB_MAX_FREQUENCIES 48
#define SI468X_DAB_DL_PLUS_MAX_TEXT_LENGTH 128

#define FREQ_MUL (10000000 / 625)

/* Command Timeouts */
#define SI468X_DEFAULT_TIMEOUT	100000
#define SI468X_TIMEOUT_TUNE	700000
#define SI468X_TIMEOUT_BOOT	300000
#define SI468X_TIMEOUT_CRC	5000000
#define SI468X_TIMEOUT_LOAD	0
#define SI468X_TIMEOUT_POWER_UP	20

/* -------------- si468x-i2c.c or si468x-spi.c ----------------- */

enum si468x_freq_supported_chips {
	SI468X_CHIP_SI4682,
	SI468X_CHIP_SI4683,
	SI468X_CHIP_SI4684,
	SI468X_CHIP_SI4688,
	SI468X_CHIP_SI4689,
};

enum si468x_mfd_cells {
	SI468X_RADIO_CELL = 0,
	SI468X_CODEC_CELL,
	SI468X_MFD_CELLS,
};

/**
 * struct si468x_device_info - device specific data
 *
 * @device_id: WHO_AM_I register's value
 * @has_am: device is able to receive am
 * @has_hd: device is able to receive hd radio
 * @has_dab: device is able to receive dab
 */
struct si468x_device_info {
	u16  device_id;
	bool has_am;
	bool has_hd;
	bool has_dab;
};

/**
 * enum si468x_power_state - possible power state of the si468x device.
 *
 * @SI468X_STATE_POWER_DOWN: In this state all regulators are turned off
 * and the reset line is pulled low. The device is completely inactive.
 * @SI468X_STATE_POWER_UP: In this state all the power regualtors are
 * turned on, reset line pulled high, IRQ line is enabled and device is
 * turned on. The device is ready for POWER_UP.
 * @SI468X_STATE_POWER_INCONSISTENT: This state indicates that previous
 * power down was inconsistent, meaning some of the regulators were
 * not turned down and thus use of the device, without power-cycling
 * is impossible.
 */
enum si468x_power_state {
	SI468X_STATE_POWER_DOWN			= 0,
	SI468X_STATE_POWER_UP			= 1,
	SI468X_STATE_POWER_INCONSISTENT		= 2,
};

/**
 * enum si468x_chip_state - possible state of the si468x device.
 *
 * @SI468X_STATE_WAITING_FOR_POWER_UP_CMD: system has been reset but
 * no POWER_UP command has been issued
 * @SI468X_STATE_RESERVED: Reserved, should not appear
 * @SI468X_STATE_BOOTLOADER_RUNNING: bootloader is currently running
 * @SI468X_STATE_APPLICATION_RUNNING: application was
 * successfully booted and is currently running
 */
#define SI468X_PUP_RESET (0)
#define SI468X_PUP_RES   (BIT(6))
#define SI468X_PUP_BOOT  (BIT(7))
#define SI468X_PUP_APP   (BIT(7) | BIT(6))
enum si468x_chip_state {
	SI468X_STATE_WAITING_FOR_POWER_UP_CMD,
	SI468X_STATE_RESERVED,
	SI468X_STATE_BOOTLOADER_RUNNING,
	SI468X_STATE_APPLICATION_RUNNING,
};

/**
 * enum si468x_nvm_state - possible nvm state of the si468x device.
 *
 * @SI468X_STATE_NVM_UNINITIALIZED: system has been reset
 * @SI468X_STATE_NVM_READY: SPI parameters are set
 */
enum si468x_nvm_state {
	SI468X_STATE_NVM_UNINITIALIZED,
	SI468X_STATE_NVM_READY,
};

/**
 * struct si468x_core - internal data structure representing the
 * underlying "core" device which all the MFD cell-devices use.
 *
 * @dev: Actual device used to transfer commands to the chip.
 * @regmap: properties shadow registers.
 * @si468x_device_info: chip model, function
 * @cells: MFD cell devices created by this driver.
 * @cmd_lock: Mutex used to serialize all the requests to the core
 * device. This filed should not be used directly. Instead
 * si468x_core_lock()/si468x_core_unlock() should be used to get
 * exclusive access to the "core" device.
 * @rds_read_queue: Wait queue used to wait for RDS data.
 * @rds_fifo: FIFO in which all the RDS data received from the chip is
 * placed.
 * @rds_fifo_drainer: Worker that drains on-chip RDS FIFO.
 * @rds_drainer_is_working: Flag used for launching only one instance
 * of the @rds_fifo_drainer.
 * @rds_drainer_status_lock: Lock used to guard access to the
 * @rds_drainer_is_working variable.
 * @update_service_list: Worker that gets new service list.
 * @update_service_data: Worker that gets new service data.
 * @digital_service_drainer_is_working: Flag used for launching only one instance
 * of the @update_service_data.
 * @digital_service_drainer_status_lock: Lock used to guard access to the
 * @digital_service_drainer_is_working variable.
 * @command: Wait queue for wainting on the command comapletion.
 * @cts: Clear To Send flag set upon receiving first status with CTS
 * set.
 * @tuning: Wait queue used for wainting for tune/seek comand
 * completion.
 * @stc: Similar to @cts, but for the STC bit of the status value.
 * @power_up_parameters: Parameters used as argument for POWER_UP
 * command when the device is started.
 * @power_state: Current power state of the device.
 * @chip_state: Current firmware state of the device.
 * @nvm_state: Current nvm state connected to the device.
 * @supplies: Structure containing handles to all power supplies used
 * by the device (NULL ones are ignored).
 * @gpio_reset: GPIO pin connected to the RSTB pin of the chip.
 * @irq: Interrupt line.
 * @bus_ops: selects how to connect to the device (I2C or SPI).
 * @is_alive: signals valid communication with the device.
 * @rds_fifo_depth: device fifos configured by the module.
 * @err: signal error when reading with CMD_RD_REPLY.
 * @response_bytes: number of bytes to read with CMD_RD_REPLY.
 * @response: bytes read with CMD_RD_REPLY.
 */

struct si468x_core {
	struct device *dev;
	struct regmap *regmap_common;
	struct regmap *regmap_am;
	struct regmap *regmap_fm;
	struct regmap *regmap_dab;
	const struct si468x_device_info *si468x_device_info;
	struct mfd_cell cells[SI468X_MFD_CELLS];

	struct mutex cmd_lock; /* for serializing fm radio operations */

	wait_queue_head_t  rds_read_queue;
	struct kfifo       rds_fifo;
	struct work_struct rds_fifo_drainer;
	bool               rds_drainer_is_working;
	struct mutex       rds_drainer_status_lock;

	struct work_struct update_service_list;
	struct work_struct update_service_data;
	bool               digital_service_drainer_is_working;
	struct mutex       digital_service_drainer_status_lock;

	wait_queue_head_t command;
	atomic_t          cts;

	wait_queue_head_t tuning;
	atomic_t          stc;

	struct si468x_power_up_args power_up_parameters;

	enum si468x_power_state power_state;
	enum si468x_chip_state  chip_state;
	enum si468x_nvm_state   nvm_state;

	struct regulator_bulk_data supplies[4];

	struct gpio_desc *gpio_reset;

	int irq;

	const struct si468x_bus_ops {
		u16 bustype;
		int (*read)(struct si468x_core *core, char *buf, int count);
		int (*write)(struct si468x_core *core, char *buf, int count);
	} *bus_ops;

	atomic_t is_alive;

	int rds_fifo_depth;

	atomic_t dab_full_scan;

	struct si468x_dab_frequency *loaded_dab_freq_list;

	char si468x_dls_message[SI468X_DAB_DL_PLUS_MAX_TEXT_LENGTH];
};

/**
 * si468x_core_lock() - lock the core device to get an exclusive access
 * to it.
 */
static inline void si468x_core_lock(struct si468x_core *core)
{
	mutex_lock(&core->cmd_lock);
}

/**
 * si468x_core_unlock() - unlock the core device to relinquish an
 * exclusive access to it.
 */
static inline void si468x_core_unlock(struct si468x_core *core)
{
	mutex_unlock(&core->cmd_lock);
}

/**
 * struct si468x_part_and_function_info - structure containing result of the
 * PART_INFO FUNC_INFO command.
 *
 * @part_info: Part Number.
 * @firmware.major: Firmware major number.
 * @firmware.minor: Firmware minor number.
 * @firmware.build: Firmware build number.
 * @svn_id: SVN ID from which the image was built.
 */
struct si468x_part_and_function_info {
	u16 part_info;
	u8 chip_revision;
	u8 rom_id;
	struct {
		u8 major, minor, build;
	} firmware;
	u32 svn_id;
};

/**
 * enum si468x_tunemode - enum representing possible tune modes for
 * the chip.
 *
 * @SI468X_TUNEMODE_FAST_NO_HD: Tune and render analog audio
 * as fast as possible, do not attempt to acquire HD.
 * @SI468X_TUNEMODE_FAST_WITH_HD: Tune and render analog audio as
 * fast as possible, try to acquire HD and crossfade to HD if
 * acquired. Always selects the Main Program Service. To select an SPS
 * use the START_DIGITAL_SERVICE command after the HD has been acquired.
 * @SI468X_TUNEMODE_HD: Tune and attempt to acquire HD and
 * render audio if successful. The Main Program Service (MPS) will be
 * rendered. To select an SPS use the START_DIGITAL_SERVICE command
 * after the HD has been acquired.
 */
enum si468x_tunemode {
	SI468X_TUNEMODE_FAST_NO_HD   = 0,
	SI468X_TUNEMODE_FAST_WITH_HD = 2,
	SI468X_TUNEMODE_HD           = 3,
};

/**
 * enum si468x_dir_tune - Enables the direct tune feature.
 *
 * @SI468X_SELECT_MAIN_PROGRAM_SERVICE: MPS is selected.
 * @SI468X_SELECT_PROGRAM_ID: The program ID specified
 * in PROG_ID will be rendered.
 */
enum si468x_dir_tune {
	SI468X_SELECT_MAIN_PROGRAM_SERVICE = 0,
	SI468X_SELECT_PROGRAM_ID           = 1,
};

/**
 * struct si468x_rds_status_report - the structure representing the
 * response to 'FM_RDS_STATUS' command
 *
 * @rdstpptyint: Traffic program flag(TP) and/or program type(PTY)
 * code has changed.
 * @rdspiint: Program identification(PI) code has changed.
 * @rdssyncint: RDS synchronization has changed.
 * @rdsfifoint: RDS was received and the RDS FIFO has at least
 * 'FM_RDS_INTERRUPT_FIFO_COUNT' elements in it.
 * @tpptyvalid: TP flag and PTY code are valid falg.
 * @pivalid: PI code is valid flag.
 * @rdssync: RDS is currently synchronized.
 * @rdsfifolost: On or more RDS groups have been lost/discarded flag.
 * @tp: Current channel's TP flag.
 * @pty: Current channel's PTY code.
 * @pi: Current channel's PI code.
 * @rdsfifoused: Number of blocks remaining in the RDS FIFO (0 if
 * empty).
 * @ble: Bit Errors corrected in blocks.
 * @rds: Block data from RDS FIFO.
 */
struct si468x_rds_status_report {
	bool rdstpptyint, rdspiint, rdssyncint, rdsfifoint;
	bool tpptyvalid, pivalid, rdssync, rdsfifolost;
	bool tp;

	u8 pty;
	u16 pi;

	u8 rdsfifoused;
	u8 ble[4];

	struct v4l2_rds_data rds[4];
};

/**
 * struct si468x_dab_frequency - the structure representing
 * one dab frequency info
 *
 * @name: name of the block
 * @frequency: mid frequency of block in kHz
 * @is_active: is part of si468x freq_list
 * @is_valid: ensemble was detected
  */
struct si468x_dab_frequency {
	const char *name;
	u32   frequency;
	bool  is_active;
	bool  is_valid;
};

struct si468x_rsq_status_args {
	bool rsqack;
	bool digradack;
	bool attune;
	bool cancel;
	bool fiberrack;
	bool stcack;
};

struct si468x_event_status_args {
	bool eventack;
};

/**
 * struct si468x_event_status_report - the structure representing the
 * response to 'DAB_GET_EVENT_STATUS' command
 *
 * @recfgint: Ensemble reconfiguration event
 * @recfgwrnint: Ensemble reconfiguration warning
 * @annoint: Announcement information
 * @oeservint: Other Ensemble (OE) Services interrupt
 * @servlinkint: new service linking information is available
 * @freqinfoint: new Frequency Information is available
 * @svrlistint: new digital service list is available
 * @anno: Announcement available
 * @oeserv: OE service information is available
 * @servlink: Service linking information available
 * @freq_info: Frequency Information (FI) is available
 * @svrlist: digital service list is available
 * @svrlistver: current version of the digital service list
 */
struct si468x_event_status_report {
	bool recfgint, recfgwrnint, annoint;
	bool oeservint, servlinkint, freqinfoint, svrlistint;
	bool anno, oeserv, servlink, freq_info, svrlist;

	u16 svrlistver;
};

struct si468x_dab_component_info {
	u8   tm_id;
	u8   sub_ch_id;
	u8   fidc_id;
	bool dg_flag;
	u8   sc_id;
	u8   audio_service_type;
	u8   data_service_type;
	bool is_primary;
	bool is_secondary;
	bool access_control_flag;
	bool mua_info_valid;
};

struct si468x_dab_service_info {
	u8   extended_country_code;
	u8   country_id;
	u32  service_id;
	bool srv_linking_info_flag;
	u8   program_type;
	bool is_data_service;
	bool is_audio_service;
	bool is_local_service;
	u8   control_access_id;
	u8   number_of_components; /* max 15 */
	u8   si_charset;
	char service_label[16+ 1];
	struct si468x_dab_component_info si468x_dab_component_info[15];
};

struct si468x_dab_service_list {
	u16  version;
	u8   number_of_services; /* max: 32 */
	struct si468x_dab_service_info si468x_dab_service_info[32];
};

/**
 * struct si468x_dab_channel - the structure representing
 * one channel
 *
 * @version: Ensemble list version
 * @frequency_index: as indexed in the device
 * @frequency: mid frequency of block in kHz
 * @fic_quality: current estimate of the ensembles
 * FIC (Fast Information Channel) quality
 * @signal_strength: Received signal strength indicator
 * @service_id: service number
 * @country_id
 * @is_data_service
 * @extended_country_code
 * @is_audio_service
 * @service_label
 * @component_info: aka Port Number or Program Number
 */
struct si468x_dab_channel {
	u16  version;
	u8   frequency_index;
	u32  frequency;
	u8   fic_quality;
	u8   signal_strength;
	u32  service_id;
	u8   country_id;
	bool is_data_service;
	u8   extended_country_code;
	bool is_audio_service;
	char service_label[16 + 1];
	struct si468x_dab_component_info component_info;
	bool is_started;
	struct list_head list;
};

enum si468x_injside {
	SI468X_INJSIDE_AUTO	= 0,
	SI468X_INJSIDE_LOW	= 1,
	SI468X_INJSIDE_HIGH	= 2,
};

struct si468x_tune_freq_args {
	enum si468x_injside injside;
	int freq;
	enum si468x_tunemode tunemode;
	int antcap;
	enum si468x_dir_tune direct_tune;
	int program_id;
	struct si468x_dab_frequency *dab_freq_list;
};

void si468x_core_stop(struct si468x_core *);
int  si468x_core_start(struct si468x_core *);
int  si473x_core_set_power_state(struct si468x_core *, enum si468x_power_state);
bool si468x_core_is_in_am_receiver_mode(struct si468x_core *);
bool si468x_core_is_in_fm_receiver_mode(struct si468x_core *);
bool si468x_core_is_in_dab_receiver_mode(struct si468x_core *);

/* -------------------- si468x-cmd.c ----------------------- */

struct si468x_core *si468x_core_probe(struct device *, int, const struct si468x_bus_ops *);
int si468x_core_remove(struct si468x_core *);
void si468x_core_suspend(struct si468x_core *);
void si468x_core_resume(struct si468x_core *);
void si468x_core_pronounce_dead(struct si468x_core *);
int si468x_core_cmd_set_property(struct si468x_core *, u16, u16);
int si468x_core_cmd_get_property(struct si468x_core *, u16);
int si468x_core_cmd_am_seek_start(struct si468x_core *,
				  const struct v4l2_hw_freq_seek *,
				  struct si468x_tune_freq_args *);
int si468x_core_cmd_fm_seek_start(struct si468x_core *,
				  const struct v4l2_hw_freq_seek *,
				  struct si468x_tune_freq_args *);
int si468x_core_cmd_dab_seek_start(struct si468x_core *,
				   const struct v4l2_hw_freq_seek *,
				   struct si468x_tune_freq_args *);
int si468x_core_cmd_fm_rds_status(struct si468x_core *, bool, bool, bool,
				  struct si468x_rds_status_report *);
int si468x_core_cmd_fm_rds_blockcount(struct si468x_core *, bool,
				      struct si468x_rds_blockcount_report *);
int si468x_core_cmd_am_tune_freq(struct si468x_core *,
				 struct si468x_tune_freq_args *);
int si468x_core_cmd_fm_tune_freq(struct si468x_core *,
				 struct si468x_tune_freq_args *);
int si468x_core_cmd_dab_tune_freq(struct si468x_core *,
				  struct si468x_tune_freq_args *);
int si468x_core_cmd_dab_set_freq_list(struct si468x_core *,
				      struct si468x_dab_frequency *, u8, u32);
int si468x_core_cmd_dab_get_freq_list(struct si468x_core *,
				      struct si468x_dab_frequency *, u32, u32);
int si468x_core_cmd_dab_event_status(struct si468x_core *,
				     struct si468x_event_status_args *,
				     struct si468x_event_status_report *);
int si468x_core_cmd_dab_get_service_list(struct si468x_core *,
					 struct si468x_dab_service_list *);
int si468x_core_cmd_am_rsq_status(struct si468x_core *,
				  struct si468x_rsq_status_args *,
				  struct si468x_rsq_status_report *);
int si468x_core_cmd_fm_rsq_status(struct si468x_core *,
				  struct si468x_rsq_status_args *,
				  struct si468x_rsq_status_report *);
int si468x_core_cmd_dab_rsq_status(struct si468x_core *,
				   struct si468x_rsq_status_args *,
				   struct si468x_rsq_status_report *);
int si468x_core_cmd_am_acf_status(struct si468x_core *,
				  struct si468x_acf_status_report *);
int si468x_core_cmd_fm_acf_status(struct si468x_core *,
				  struct si468x_acf_status_report *);
int si468x_core_cmd_dab_acf_status(struct si468x_core *,
				   struct si468x_acf_status_report *);
int si468x_core_cmd_agc_status(struct si468x_core *,
			       struct si468x_agc_status_report *);

/* Properties  */

enum si468x_interrupt_flags {
	SI468X_STCIEN  = BIT(0),
	SI468X_ACFIEN  = BIT(1),
	SI468X_RDSIEN  = BIT(2),
	SI468X_RSQIEN  = BIT(3),
	SI468X_DSRVIEN = BIT(4),
	SI468X_DACQIEN = BIT(5),
	SI468X_ERRIEN  = BIT(6),
	SI468X_CTSIEN  = BIT(7),
	SI468X_DEVNTIEN= BIT(13),
};

enum si468x_interrupt_repeat_flags {
	SI468X_STCREP   = BIT(0),
	SI468X_ACFREP   = BIT(1),
	SI468X_RDSREP   = BIT(2),
	SI468X_RSQREP   = BIT(3),
	SI468X_DSRVREP  = BIT(4),
	SI468X_DACQREP  = BIT(5),
	SI468X_DEVNTREP = BIT(13),
};

enum si468x_rdsint_sources {
	SI468X_RDSRECV  = BIT(0),
	SI468X_RDSSYNC  = BIT(1),
	SI468X_RDSPI    = BIT(3),
	SI468X_RDSTPPTY = BIT(4),
};

enum si468x_status0_response_bits {
	SI468X_STC_INT    = BIT(0), /* Seek/Tune complete */
	SI468X_ACF_INT    = BIT(1), /* Automatically controlled features indicator */
	SI468X_FM_RDS_INT = BIT(2), /* RDS Data indicator */
	SI468X_RSQ_INT    = BIT(3), /* Received Signal Quality indicator */
	SI468X_DSRV_INT   = BIT(4), /* enabled data component requires attention */
	SI468X_DACQ_INT   = BIT(5), /* Digital radio link change indicator */
	SI468X_ERR        = BIT(6), /* Command Error */
	SI468X_CTS        = BIT(7), /* Clear to Send */
};

enum si468x_status1_response_bits {
	SI468X_DACF_INT   = BIT(0), /* HD radio ACF status change indicator */
	SI468X_DEVNT_INT  = BIT(5), /* Digital radio event change indicator */
};

enum si468x_status3_response_bits {
	SI468X_ERRNR      = BIT(0), /* non-recoverable error */
	SI468X_ARBERR     = BIT(1), /* arbiter error has occurred */
	SI468X_CMDOFERR   = BIT(2), /* control interface has dropped cmd */
	SI468X_REPOFERR   = BIT(3), /* control interface has dropped response */
	SI468X_DSPERR     = BIT(4), /* DSP has encountered a frame overrun */
	SI468X_RFFE_ERR   = BIT(5), /* RF front end is in unexpected state */
	SI468X_PUP_MASK   = BIT(7) | BIT(6), /* powerup state */
};

static const struct si468x_device_info si468x_device_info_table[] = {
	[SI468X_CHIP_SI4682] = {
		.device_id = 4682,
		.has_am    = false,
		.has_hd    = true,
		.has_dab   = false,
	},
	[SI468X_CHIP_SI4683] = {
		.device_id = 4683,
		.has_am    = true,
		.has_hd    = true,
		.has_dab   = false,
	},
	[SI468X_CHIP_SI4684] = {
		.device_id = 4684,
		.has_am    = false,
		.has_hd    = false,
		.has_dab   = true,
	},
	[SI468X_CHIP_SI4688] = {
		.device_id = 4688,
		.has_am    = false,
		.has_hd    = true,
		.has_dab   = true,
	},
	[SI468X_CHIP_SI4689] = {
		.device_id = 4689,
		.has_am    = true,
		.has_hd    = true,
		.has_dab   = true,
	},
};

static const struct of_device_id of_si468x_match[] = {
	{ .compatible = "si,si4682", .data = &si468x_device_info_table[SI468X_CHIP_SI4682]},
	{ .compatible = "si,si4683", .data = &si468x_device_info_table[SI468X_CHIP_SI4683]},
	{ .compatible = "si,si4684", .data = &si468x_device_info_table[SI468X_CHIP_SI4684]},
	{ .compatible = "si,si4688", .data = &si468x_device_info_table[SI468X_CHIP_SI4688]},
	{ .compatible = "si,si4689", .data = &si468x_device_info_table[SI468X_CHIP_SI4689]},
	{ /*sentinel*/ },
};

/* -------------------- si468x-prop.c ----------------------- */

enum si468x_common_receiver_properties {
/* Interrupt enable property */
	SI468X_PROP_INT_CTL_ENABLE = 0x0000,
/* Interrupt repeat property */
	SI468X_PROP_INT_CTL_REPEAT = 0x0001,
/* Selects digital audio Master or Slave. */
	SI468X_PROP_DIGITAL_IO_OUTPUT_SELECT = 0x0200,
/* Sets output sample audio rate in units of 1Hz. */
	SI468X_PROP_DIGITAL_IO_OUTPUT_SAMPLE_RATE = 0x0201,
/* Configure digital output format. */
	SI468X_PROP_DIGITAL_IO_OUTPUT_FORMAT = 0x0202,
/* Deviations from the standard framing mode */
	SI468X_PROP_DIGITAL_IO_OUTPUT_FORMAT_OVERRIDES_1 = 0x0203,
/* Deviations from the standard framing mode */
	SI468X_PROP_DIGITAL_IO_OUTPUT_FORMAT_OVERRIDES_2 = 0x0204,
/* Deviations from the standard framing mode */
	SI468X_PROP_DIGITAL_IO_OUTPUT_FORMAT_OVERRIDES_3 = 0x0205,
/* Deviations from the standard framing mode */
	SI468X_PROP_DIGITAL_IO_OUTPUT_FORMAT_OVERRIDES_4 = 0x0206,
/* Sets the audio analog volume. */
	SI468X_PROP_AUDIO_ANALOG_VOLUME = 0x0300,
/* AUDIO_MUTE property mutes/unmutes each audio output independently. */
	SI468X_PROP_AUDIO_MUTE = 0x0301,
/* AUDIO_OUTPUT_CONFIG is used to configure various settings of the audio output. */
	SI468X_PROP_AUDIO_OUTPUT_CONFIG = 0x0302,
/* Pin configuration property */
	SI468X_PROP_PIN_CONFIG_ENABLE = 0x0800,
/* Enables the wake tone feature. */
	SI468X_PROP_WAKE_TONE_ENABLE = 0x0900,
/* Sets the wake tone duty cycle. */
	SI468X_PROP_WAKE_TONE_PERIOD = 0x0901,
/* Sets the wake tone frequency. */
	SI468X_PROP_WAKE_TONE_FREQ = 0x0902,
/* Sets the wake tone amplitude. */
	SI468X_PROP_WAKE_TONE_AMPLITUDE = 0x0903,
/* Configures the interrupt sources for digital services */
	SI468X_PROP_DIGITAL_SERVICE_INT_SOURCE = 0x8100,
/* sets the delay time (in miliseconds) to restart digital service when recovering from acquisition loss */
	SI468X_PROP_DIGITAL_SERVICE_RESTART_DELAY = 0x8101,
/* Enables the HD BER test. */
	SI468X_PROP_HD_TEST_BER_CONFIG = 0xE800,
};

enum si468x_am_receiver_properties {
/* Sets the minimum gain for automatic volume control.  */
	SI468X_PROP_AM_AVC_MIN_GAIN = 0x0500,
/* Sets the maximum gain for automatic volume control. */
	SI468X_PROP_AM_AVC_MAX_GAIN = 0x0501,
/* The SNR/RSSI threshold for maximum and minimum channel filter bandwidth */
	SI468X_PROP_AM_CHBW_SQ_LIMITS = 0x2200,
/* The maximum and minimum channel filter bandwidth. */
	SI468X_PROP_AM_CHBW_SQ_CHBW = 0x2201,
/* SNR/RSSI based channel filter bandwidth widening time */
	SI468X_PROP_AM_CHBW_SQ_WIDENING_TIME = 0x2202,
/* SNR/RSSI based channel filter bandwidth narrowing time */
	SI468X_PROP_AM_CHBW_SQ_NARROWING_TIME = 0x2203,
/* Overrides the automatically controlled channel filter setting. */
	SI468X_PROP_AM_CHBW_OVERRIDE_BW = 0x2204,
/* Sets the lower seek boundary of the AM band in multiples of 1kHz. */
	SI468X_PROP_AM_SEEK_BAND_BOTTOM = 0x4100,
/* Sets the upper seek boundary for the AM band in multiples of 1kHz. */
	SI468X_PROP_AM_SEEK_BAND_TOP = 0x4101,
/* Sets the frequency spacing for the AM band in multiples of 1kHz when performing a seek. */
	SI468X_PROP_AM_SEEK_FREQUENCY_SPACING = 0x4102,
/* Sets the maximum frequency error allowed before setting the AFCRL indicator. */
	SI468X_PROP_AM_VALID_MAX_TUNE_ERROR = 0x4200,
/* Sets the amount of time in ms to allow the RSSI/ISSI metrics to settle before evaluating. */
	SI468X_PROP_AM_VALID_RSSI_TIME = 0x4201,
/* Sets the RSSI threshold for a valid AM Seek/Tune. */
	SI468X_PROP_AM_VALID_RSSI_THRESHOLD = 0x4202,
/* Sets the amount of time in ms to allow the SNR metric to settle before evaluating. */
	SI468X_PROP_AM_VALID_SNR_TIME = 0x4203,
/* Sets the SNR threshold for a valid AM Seek/Tune. */
	SI468X_PROP_AM_VALID_SNR_THRESHOLD = 0x4204,
/* Sets the HDLEVEL threshold for a valid AM Seek/Tune. */
	SI468X_PROP_AM_VALID_HDLEVEL_THRESHOLD = 0x4205,
/* Configures interrupt related to Received Signal Quality metrics. */
	SI468X_PROP_AM_RSQ_INTERRUPT_SOURCE = 0x4300,
/* AM_RSQ_SNR_HIGH_THRESHOLD sets the high threshold, which triggers the RSQ interrupt if the SNR is above this threshold. */
	SI468X_PROP_AM_RSQ_SNR_HIGH_THRESHOLD = 0x4301,
/* AM_RSQ_SNR_LOW_THRESHOLD sets the low threshold, which triggers the RSQ interrupt if the SNR is below this threshold.  */
	SI468X_PROP_AM_RSQ_SNR_LOW_THRESHOLD = 0x4302,
/* AM_RSQ_RSSI_HIGH_THRESHOLD sets the high threshold, which triggers the RSQ interrupt if the RSSI is above this threshold. */
	SI468X_PROP_AM_RSQ_RSSI_HIGH_THRESHOLD = 0x4303,
/* Sets low threshold which triggers the RSQ interrupt if the RSSI is below this threshold.  */
	SI468X_PROP_AM_RSQ_RSSI_LOW_THRESHOLD = 0x4304,
/* Configures the Fast HD Detection routine. */
	SI468X_PROP_AM_RSQ_HD_DETECTION = 0x4305,
/* Configures the Fast HD Detection Level Metric Filtering Time Constant. */
	SI468X_PROP_AM_RSQ_HD_LEVEL_TIME_CONST = 0x4306,
/* Configures the HD Level Detected Threshold. */
	SI468X_PROP_AM_RSQ_HDDETECTED_THD = 0x4307,
/* Sets high threshold which triggers the RSQ interrupt if FILTERED_HDLEVEL is above this threshold. */
	SI468X_PROP_AM_RSQ_HDLEVEL_HIGH_THRESHOLD = 0x4308,
/* Sets low threshold which triggers the RSQ interrupt if FILTERED_HDLEVEL is below this threshold. */
	SI468X_PROP_AM_RSQ_HDLEVEL_LOW_THRESHOLD = 0x4309,
/* Enables the ACF interrupt sources. */
	SI468X_PROP_AM_ACF_INTERRUPT_SOURCE = 0x4400,
/* Sets the softmute interrupt threshold in dB attenuation. */
	SI468X_PROP_AM_ACF_SOFTMUTE_THRESHOLD = 0x4401,
/* Sets the high cut interrupt threshold. */
	SI468X_PROP_AM_ACF_HIGHCUT_THRESHOLD = 0x4402,
/* Sets the distance from the final softmute value that triggers softmute convergence flag. */
	SI468X_PROP_AM_ACF_SOFTMUTE_TOLERANCE = 0x4403,
/* Sets the distance from the final high cut frequency that triggers the high cut convergence flag. */
	SI468X_PROP_AM_ACF_HIGHCUT_TOLERANCE = 0x4404,
/* Determines what metrics control various ACF features. */
	SI468X_PROP_AM_ACF_CONTROL_SOURCE = 0x4405,
/* Sets the SNR limits for soft mute attenuation. */
	SI468X_PROP_AM_SOFTMUTE_SQ_LIMITS = 0x4500,
/* Sets the SNR based softmute attenuation limits. */
	SI468X_PROP_AM_SOFTMUTE_SQ_ATTENUATION = 0x4501,
/* Sets the soft mute attack time. */
	SI468X_PROP_AM_SOFTMUTE_SQ_ATTACK_TIME = 0x4502,
/* Sets the soft mute release time. */
	SI468X_PROP_AM_SOFTMUTE_SQ_RELEASE_TIME = 0x4503,
/* SQ based high cut high threshold. */
	SI468X_PROP_AM_HIGHCUT_SQ_LIMITS = 0x4600,
/* Max and Min audio frequencies */
	SI468X_PROP_AM_HIGHCUT_SQ_CUTOFF_FREQ = 0x4601,
/* Hi-cut cutoff frequency attack time in ms. */
	SI468X_PROP_AM_HIGHCUT_SQ_ATTACK_TIME = 0x4602,
/* Hi-cut cutoff frequency release time in ms. */
	SI468X_PROP_AM_HIGHCUT_SQ_RELEASE_TIME = 0x4603,
/* Specify the AFC range */
	SI468X_PROP_AM_DEMOD_AFC_RANGE = 0x4800,
/* Sets the DAAI/CDNO level at which digital audio bandwidth management begins to engage. */
	SI468X_PROP_HD_BLEND_BWM_CTRL_THRES = 0x9120,
/* Sets the blend starting point and the maximum bandwidth of digital audio signal in 100Hz. */
	SI468X_PROP_HD_BLEND_BWM_CTRL_LEVEL = 0x9121,
/* Bandwidth management cutoff frequency ramp up time in ms. */
	SI468X_PROP_HD_BLEND_BWM_CTRL_RAMP_UP_TIME = 0x9122,
/* Bandwidth management cutoff frequency ramp down time in ms. */
	SI468X_PROP_HD_BLEND_BWM_CTRL_RAMP_DOWN_TIME = 0x9123,
/* Sets the bandwidth level at which digital audio mono stereo transition begins to engage. */
	SI468X_PROP_HD_BLEND_BWM_BLEND_THRES = 0x9124,
/* Sets the min and max stereo separation value in dB at which the blend starts to engage. */
	SI468X_PROP_HD_BLEND_BWM_BLEND_LEVEL = 0x9125,
/* Bandwidth management mono stereo transition ramp up time in ms. */
	SI468X_PROP_HD_BLEND_BWM_BLEND_RAMP_UP_TIME = 0x9126,
/* Bandwidth management mono stereo transition ramp down time in ms. */
	SI468X_PROP_HD_BLEND_BWM_BLEND_RAMP_DOWN_TIME = 0x9127,
/* Sets the HD audio bandwidth interrupt threshold. */
	SI468X_PROP_HD_ACF_AUDIO_BW_THRESHOLD = 0x9402,
/* Sets the HD audio stereo separation interrupt threshold. */
	SI468X_PROP_HD_ACF_STEREO_SEP_THRESHOLD = 0x9403,
/* Sets the distance from the final HD audio bandwidth that triggers the hd audio bandwidth convergence flag. */
	SI468X_PROP_HD_ACF_AUDIO_BW_TOLERANCE = 0x9405,
/* Sets the distance from the final HD audio stereo separation that triggers the hd audio stereo separation convergence flag. */
	SI468X_PROP_HD_ACF_STEREO_SEP_TOLERANCE = 0x9406,
/* Configures AM enhanced stream holdoff. */
	SI468X_PROP_HD_ENHANCED_STREAM_HOLDOFF_CONFIG = 0x9F00,
/* Sets the enhanced stream holdoff thresholds. */
	SI468X_PROP_HD_ENHANCED_STREAM_HOLDOFF_THRESHOLDS = 0x9F01,
};

enum si468x_am_fm_receiver_properties {
/* Adjusts the midpoint of threshold for the RF Threshold Detector. */
	SI468X_PROP_AGC_RF_THRESHOLD = 0x170C,
/* This property provides options to control HD/analog audio blend behavior. This property is only valid for Hybrid (non-All-Digital HD) Broadcasts. */
	SI468X_PROP_HD_BLEND_OPTIONS = 0x9101,
/* This property sets the amount of time it takes in ms to blend from analog to HD. This property only applies to primary service channel. */
	SI468X_PROP_HD_BLEND_ANALOG_TO_HD_TRANSITION_TIME = 0x9102,
/* This property sets the amount of time it takes in ms to blend from HD to analog. This property only applies to primary service channel. */
	SI468X_PROP_HD_BLEND_HD_TO_ANALOG_TRANSITION_TIME = 0x9103,
/* This property sets the digital audio dynamic linear scaling factor. Setting DGAIN_OVERRIDE bit to 1 will override the broadcaster specified digital gain. The DGAIN field is a signed 8 bit fractional number where the linear gain is equal to DGAIN/128. */
	SI468X_PROP_HD_BLEND_DYNAMIC_GAIN = 0x9106,
/* This property defines the analog to digital blend threshold. */
	SI468X_PROP_HD_BLEND_DECISION_ANA_TO_DIG_THRESHOLD = 0x9109,
/* This property defines the analog to digital blend delay. */
	SI468X_PROP_HD_BLEND_DECISION_ANA_TO_DIG_DELAY = 0x910A,
/* Sets the service re-acquisition unmute time in ms. */
	SI468X_PROP_HD_BLEND_SERV_LOSS_RAMP_UP_TIME = 0x910B,
/* Sets the service switching mute time in ms. */
	SI468X_PROP_HD_BLEND_SERV_LOSS_RAMP_DOWN_TIME = 0x910C,
/* Sets the comfort noise unmute time in ms. */
	SI468X_PROP_HD_BLEND_SERV_LOSS_NOISE_RAMP_UP_TIME = 0x910D,
/* Sets the comfort noise mute time in ms. */
	SI468X_PROP_HD_BLEND_SERV_LOSS_NOISE_RAMP_DOWN_TIME = 0x910E,
/* Sets the service loss noise level. */
	SI468X_PROP_HD_BLEND_SERV_LOSS_NOISE_LEVEL = 0x910F,
/* Sets the DAAI threshold below which comfort noise will engage. */
	SI468X_PROP_HD_BLEND_SERV_LOSS_NOISE_DAAI_THRESHOLD = 0x9110,
/* Sets the audio restart delay used by the comfort noise ramping algorithm. */
	SI468X_PROP_HD_BLEND_SERV_LOSS_NOISE_AUDIO_START_DELAY = 0x9111,
/* Configures interrupts related to digital receivers HD_DIGRAD_STATUS command. */
	SI468X_PROP_HD_DIGRAD_INTERRUPT_SOURCE = 0x9200,
/* Configures CDNR LOW interrupt interrupt threshold. */
	SI468X_PROP_HD_DIGRAD_CDNR_LOW_THRESHOLD = 0x9201,
/* Configures CDNR HIGH interrupt interrupt threshold. */
	SI468X_PROP_HD_DIGRAD_CDNR_HIGH_THRESHOLD = 0x9202,
/* Configures interrupts related to digital receiver (HD_GET_EVENT_STATUS). */
	SI468X_PROP_HD_EVENT_INTERRUPT_SOURCE = 0x9300,
/* This property configures which basic SIS information is returned by the HD_GET_STATION_INFO command BASICSIS option. */
	SI468X_PROP_HD_EVENT_SIS_CONFIG = 0x9301,
/* This property configures HD alerts. Alert information is returned by the HD_GET_ALERT_MSG command. */
	SI468X_PROP_HD_EVENT_ALERT_CONFIG = 0x9302,
/* Enables the HD ACF interrupt sources. */
	SI468X_PROP_HD_ACF_INTERRUPT_SOURCE = 0x9400,
/* Sets the comfort noise interrupt threshold. */
	SI468X_PROP_HD_ACF_COMF_NOISE_THRESHOLD = 0x9401,
/* Sets the distance from the final comfort noise level that triggers the comfort noise convergence flag. */
	SI468X_PROP_HD_ACF_COMF_NOISE_TOLERANCE = 0x9404,
/* Sets which audio services will provide program service data. */
	SI468X_PROP_HD_PSD_ENABLE = 0x9500,
/* Sets which PSD fields will be decoded. */
	SI468X_PROP_HD_PSD_FIELD_MASK = 0x9501,
/* Controls the value of the delay of decoded digital audio samples relative to the output of the audio quality indicator. */
	SI468X_PROP_HD_AUDIO_CTRL_FRAME_DELAY = 0x9700,
/* Controls the duration before reverting to MPS audio after an SPS audio program is removed or lost. */
	SI468X_PROP_HD_AUDIO_CTRL_PROGRAM_LOSS_THRESHOLD = 0x9701,
/* Selects the audio output for hybrid waveforms when the TX Blend Control Status (BCTL) bits are set to 01. */
	SI468X_PROP_HD_AUDIO_CTRL_BALL_GAME_ENABLE = 0x9702,
/* Blend threshold for Codec Mode 0 */
	SI468X_PROP_HD_CODEC_MODE_0_BLEND_THRESHOLD = 0x9900,
/* Digital Audio Delay for Codec Mode 0 */
	SI468X_PROP_HD_CODEC_MODE_0_SAMPLES_DELAY = 0x9901,
/* Blend Rate for Codec Mode 0 */
	SI468X_PROP_HD_CODEC_MODE_0_BLEND_RATE = 0x9902,
/* Blend threshold for Codec Mode 2 */
	SI468X_PROP_HD_CODEC_MODE_2_BLEND_THRESHOLD = 0x9903,
/* Digital Audio Delay for Codec Mode 2 */
	SI468X_PROP_HD_CODEC_MODE_2_SAMPLES_DELAY = 0x9904,
/* Blend Rate for Codec Mode 2 */
	SI468X_PROP_HD_CODEC_MODE_2_BLEND_RATE = 0x9905,
/* Blend threshold for Codec Mode 10 */
	SI468X_PROP_HD_CODEC_MODE_10_BLEND_THRESHOLD = 0x9906,
/* Digital Audio Delay for Codec Mode 10 */
	SI468X_PROP_HD_CODEC_MODE_10_SAMPLES_DELAY = 0x9907,
/* Blend Rate for Codec Mode 10 */
	SI468X_PROP_HD_CODEC_MODE_10_BLEND_RATE = 0x9908,
/* Blend threshold for Codec Mode 13 */
	SI468X_PROP_HD_CODEC_MODE_13_BLEND_THRESHOLD = 0x9909,
/* Digital Audio Delay for Codec Mode 13 */
	SI468X_PROP_HD_CODEC_MODE_13_SAMPLES_DELAY = 0x990A,
/* Blend Rate for Codec Mode 13 */
	SI468X_PROP_HD_CODEC_MODE_13_BLEND_RATE = 0x990B,
/* Blend threshold for Codec Mode 1 */
	SI468X_PROP_HD_CODEC_MODE_1_BLEND_THRESHOLD = 0x990C,
/* Digital Audio Delay for Codec Mode 1 */
	SI468X_PROP_HD_CODEC_MODE_1_SAMPLES_DELAY = 0x990D,
/* Blend Rate for Codec Mode 1 */
	SI468X_PROP_HD_CODEC_MODE_1_BLEND_RATE = 0x990E,
/* Blend threshold for Codec Mode 3 */
	SI468X_PROP_HD_CODEC_MODE_3_BLEND_THRESHOLD = 0x990F,
/* Digital Audio Delay for Codec Mode 3 */
	SI468X_PROP_HD_CODEC_MODE_3_SAMPLES_DELAY = 0x9910,
/* Blend Rate for Codec Mode 3 */
	SI468X_PROP_HD_CODEC_MODE_3_BLEND_RATE = 0x9911,
/* This property enables and disables HD EZ blend. */
	SI468X_PROP_HD_EZBLEND_ENABLE = 0x9B00,
/* This property sets the threshold for determining when to blend between digital audio and analog audio for Hybrid MPS. */
	SI468X_PROP_HD_EZBLEND_MPS_BLEND_THRESHOLD = 0x9B01,
/* This property configures the hysteresis in the blending process for Hybrid MPS. */
	SI468X_PROP_HD_EZBLEND_MPS_BLEND_RATE = 0x9B02,
/* This property is used to perform audio alignment between analog and Hybrid MPS digital audio. */
	SI468X_PROP_HD_EZBLEND_MPS_SAMPLES_DELAY = 0x9B03,
/* This property sets the threshold for determining when to blend between digital audio and mute for SPS programs as well as All Digital MPS programs. */
	SI468X_PROP_HD_EZBLEND_SPS_BLEND_THRESHOLD = 0x9B04,
/* This property configures the hysteresis in the blending process for SPS programs and All Digital MPS programs. */
	SI468X_PROP_HD_EZBLEND_SPS_BLEND_RATE = 0x9B05,
};

enum si468x_fm_receiver_properties {
/* FM Front End Varactor configuration slope */
	SI468X_PROP_FM_TUNE_FE_VARM = 0x1710,
/* FM Front End Varactor configuration intercept */
	SI468X_PROP_FM_TUNE_FE_VARB = 0x1711,
/* Additional configuration options for the front end. */
	SI468X_PROP_FM_TUNE_FE_CFG = 0x1712,
/* Sets the lower seek boundary of the FM band in multiples of 10kHz. */
	SI468X_PROP_FM_SEEK_BAND_BOTTOM = 0x3100,
/* Sets the upper seek boundary for the FM band in multiples of 10kHz. */
	SI468X_PROP_FM_SEEK_BAND_TOP = 0x3101,
/* Sets the frequency spacing for the FM band in multiples of 10kHz when performing a seek. */
	SI468X_PROP_FM_SEEK_FREQUENCY_SPACING = 0x3102,
/* Sets the maximum frequency error allowed before setting the AFCRL indicator. */
	SI468X_PROP_FM_VALID_MAX_TUNE_ERROR = 0x3200,
/* Sets the amount of time in ms to allow the RSSI/ISSI metrics to settle before evaluating. */
	SI468X_PROP_FM_VALID_RSSI_TIME = 0x3201,
/* Sets the RSSI threshold for a valid FM Seek/Tune. */
	SI468X_PROP_FM_VALID_RSSI_THRESHOLD = 0x3202,
/* Sets the amount of time in ms to allow the SNR metric to settle before evaluating. */
	SI468X_PROP_FM_VALID_SNR_TIME = 0x3203,
/* Sets the SNR threshold for a valid FM Seek/Tune. */
	SI468X_PROP_FM_VALID_SNR_THRESHOLD = 0x3204,
/* Sets the HDLEVEL threshold for a valid FM Seek/Tune. */
	SI468X_PROP_FM_VALID_HDLEVEL_THRESHOLD = 0x3206,
/* Configures interrupt related to Received Signal Quality metrics. */
	SI468X_PROP_FM_RSQ_INTERRUPT_SOURCE = 0x3300,
/* sets the high threshold, which triggers the RSQ interrupt if the SNR is above this threshold. */
	SI468X_PROP_FM_RSQ_SNR_HIGH_THRESHOLD = 0x3301,
/* sets the low threshold, which triggers the RSQ interrupt if the SNR is below this threshold.  */
	SI468X_PROP_FM_RSQ_SNR_LOW_THRESHOLD = 0x3302,
/* sets the high threshold, which triggers the RSQ interrupt if the RSSI is above this threshold. */
	SI468X_PROP_FM_RSQ_RSSI_HIGH_THRESHOLD = 0x3303,
/* Sets low threshold which triggers the RSQ interrupt if the RSSI is below this threshold.  */
	SI468X_PROP_FM_RSQ_RSSI_LOW_THRESHOLD = 0x3304,
/* Configures the Fast HD Detection routine. */
	SI468X_PROP_FM_RSQ_HD_DETECTION = 0x3307,
/* Configures the Fast HD Detection Level Metric Filtering Time Constant. */
	SI468X_PROP_FM_RSQ_HD_LEVEL_TIME_CONST = 0x3308,
/* Configures the HD Level Detected Threshold. */
	SI468X_PROP_FM_RSQ_HDDETECTED_THD = 0x3309,
/* Sets high threshold which triggers the RSQ interrupt if FILTERED_HDLEVEL is above this threshold. */
	SI468X_PROP_FM_RSQ_HDLEVEL_HIGH_THRESHOLD = 0x330A,
/* Sets low threshold which triggers the RSQ interrupt if FILTERED_HDLEVEL is below this threshold. */
	SI468X_PROP_FM_RSQ_HDLEVEL_LOW_THRESHOLD = 0x330B,
/* Enables the ACF interrupt sources. */
	SI468X_PROP_FM_ACF_INTERRUPT_SOURCE = 0x3400,
/* Sets the softmute interrupt threshold in dB attenuation. */
	SI468X_PROP_FM_ACF_SOFTMUTE_THRESHOLD = 0x3401,
/* Sets the high cut interrupt threshold. */
	SI468X_PROP_FM_ACF_HIGHCUT_THRESHOLD = 0x3402,
/* Sets the stereo blend interrupt threshold. */
	SI468X_PROP_FM_ACF_BLEND_THRESHOLD = 0x3403,
/* Sets the distance from the final softmute value that triggers softmute convergence flag. */
	SI468X_PROP_FM_ACF_SOFTMUTE_TOLERANCE = 0x3404,
/* Sets the distance from the final high cut frequency that triggers the high cut convergence flag. */
	SI468X_PROP_FM_ACF_HIGHCUT_TOLERANCE = 0x3405,
/* Sets the distance from the final blend state that triggers the blend convergence flag. */
	SI468X_PROP_FM_ACF_BLEND_TOLERANCE = 0x3406,
/* Sets the SNR limits for soft mute attenuation */
	SI468X_PROP_FM_SOFTMUTE_SNR_LIMITS = 0x3500,
/* Sets the SNR based softmute attenuation limits. */
	SI468X_PROP_FM_SOFTMUTE_SNR_ATTENUATION = 0x3501,
/* Sets the soft mute attack time. */
	SI468X_PROP_FM_SOFTMUTE_SNR_ATTACK_TIME = 0x3502,
/* Sets the soft mute release time. */
	SI468X_PROP_FM_SOFTMUTE_SNR_RELEASE_TIME = 0x3503,
/* RSSI based high cut limits. */
	SI468X_PROP_FM_HIGHCUT_RSSI_LIMITS = 0x3600,
/* RSSI based high cut cutoff frequencies. */
	SI468X_PROP_FM_HIGHCUT_RSSI_CUTOFF_FREQ = 0x3601,
/* RSSI based high cut attack time. */
	SI468X_PROP_FM_HIGHCUT_RSSI_ATTACK_TIME = 0x3602,
/* RSSI based high cut release time. */
	SI468X_PROP_FM_HIGHCUT_RSSI_RELEASE_TIME = 0x3603,
/* SNR based high cut limits. */
	SI468X_PROP_FM_HIGHCUT_SNR_LIMITS = 0x3604,
/* SNR based high cut cutoff frequencies. */
	SI468X_PROP_FM_HIGHCUT_SNR_CUTOFF_FREQ = 0x3605,
/* SNR based high cut attack time. */
	SI468X_PROP_FM_HIGHCUT_SNR_ATTACK_TIME = 0x3606,
/* SNR based high cut release time. */
	SI468X_PROP_FM_HIGHCUT_SNR_RELEASE_TIME = 0x3607,
/* Multipath based high cut limits. */
	SI468X_PROP_FM_HIGHCUT_MULTIPATH_LIMITS = 0x3608,
/* Multipath based high cut cutoff frequencies. */
	SI468X_PROP_FM_HIGHCUT_MULTIPATH_CUTOFF_FREQ = 0x3609,
/* Multipath based high cut attack time. */
	SI468X_PROP_FM_HIGHCUT_MULTIPATH_ATTACK_TIME = 0x360A,
/* Multipath based high cut release time. */
	SI468X_PROP_FM_HIGHCUT_MULTIPATH_RELEASE_TIME = 0x360B,
/* This property sets the RSSI limits for RSSI controlled stereo separation. */
	SI468X_PROP_FM_BLEND_RSSI_LIMITS = 0x3700,
/* RSSI based stereo to mono attack time in ms. */
	SI468X_PROP_FM_BLEND_RSSI_ATTACK_TIME = 0x3702,
/* RSSI based mono to stereo release time in ms. */
	SI468X_PROP_FM_BLEND_RSSI_RELEASE_TIME = 0x3703,
/* Sets the SNR limits for SNR controlled stereo separation. */
	SI468X_PROP_FM_BLEND_SNR_LIMITS = 0x3704,
/* SNR based stereo to mono attack time in ms. */
	SI468X_PROP_FM_BLEND_SNR_ATTACK_TIME = 0x3706,
/* SNR based mono to stereo release time in ms. */
	SI468X_PROP_FM_BLEND_SNR_RELEASE_TIME = 0x3707,
/* Sets the multipath limits for multipath controlled stereo separation. */
	SI468X_PROP_FM_BLEND_MULTIPATH_LIMITS = 0x3708,
/* Multi-path based stereo to mono attack time in ms. */
	SI468X_PROP_FM_BLEND_MULTIPATH_ATTACK_TIME = 0x370A,
/* Multi-path based mono to stereo release time in ms. */
	SI468X_PROP_FM_BLEND_MULTIPATH_RELEASE_TIME = 0x370B,
/* Sets the FM Receive de-emphasis. */
	SI468X_PROP_FM_AUDIO_DE_EMPHASIS = 0x3900,
/* This property configures interrupt related to RDS.. */
	SI468X_PROP_FM_RDS_INTERRUPT_SOURCE = 0x3C00,
/* Configures minimum received data groups in fifo before interrupt. */
	SI468X_PROP_FM_RDS_INTERRUPT_FIFO_COUNT = 0x3C01,
/* Enables RDS and configures acceptable block error threshold. */
	SI468X_PROP_FM_RDS_CONFIG = 0x3C02,
/* Configures rds block confidence threshold. */
	SI468X_PROP_FM_RDS_CONFIDENCE = 0x3C03,
/* Sets the service switching unmute time in ms. */
	SI468X_PROP_HD_BLEND_SERV_SWITCH_RAMP_UP_TIME = 0x9112,
/* Sets the service switching mute time in ms. */
	SI468X_PROP_HD_BLEND_SERV_SWITCH_RAMP_DOWN_TIME = 0x9113,
/* This property Enables MP11 mode support. If MP11 support is disabled using this property the receiver will fall back to MP3 mode of operation when tuned to a station that is transmitting the MP11 subcarriers. */
	SI468X_PROP_HD_SERVICE_MODE_CONTROL_MP11_ENABLE = 0x9A00,
/* Used to put the analog audio output in to special test modes for debug purposes. */
	SI468X_PROP_HD_TEST_DEBUG_AUDIO = 0xE801,
};

enum si468x_dab_receiver_properties {
/* DAB/DMB Front End Varactor configuration slope */
	SI468X_PROP_DAB_TUNE_FE_VARM = 0x1710,
/* DAB/DMB Front End Varactor configuration intercept */
	SI468X_PROP_DAB_TUNE_FE_VARB = 0x1711,
/* Additional configuration options for the front end. */
	SI468X_PROP_DAB_TUNE_FE_CFG = 0x1712,
/* Configures interrupts related to digital receiver. */
	SI468X_PROP_DAB_DIGRAD_INTERRUPT_SOURCE = 0xB000,
/* DAB_DIGRAD_RSSI_HIGH_THRESHOLD sets the high threshold, which triggers the DIGRAD interrupt if the RSSI is above this threshold. */
	SI468X_PROP_DAB_DIGRAD_RSSI_HIGH_THRESHOLD = 0xB001,
/* DAB_DIGRAD_RSSI_LOW_THRESHOLD sets the low threshold, which triggers the DIGRAD interrupt if the RSSI is below this threshold.  */
	SI468X_PROP_DAB_DIGRAD_RSSI_LOW_THRESHOLD = 0xB002,
/* Sets the time to allow the RSSI metric to settle before evaluating. */
	SI468X_PROP_DAB_VALID_RSSI_TIME = 0xB200,
/* Sets the RSSI threshold for a valid DAB Seek/Tune */
	SI468X_PROP_DAB_VALID_RSSI_THRESHOLD = 0xB201,
/* Set the time to wait for acquisition before evaluating */
	SI468X_PROP_DAB_VALID_ACQ_TIME = 0xB202,
/* Sets the time to wait for ensemble synchronization. */
	SI468X_PROP_DAB_VALID_SYNC_TIME = 0xB203,
/* Sets the time to wait for ensemble fast signal detection. */
	SI468X_PROP_DAB_VALID_DETECT_TIME = 0xB204,
/* Configures which dab events will set the DEVENTINT status bit. */
	SI468X_PROP_DAB_EVENT_INTERRUPT_SOURCE = 0xB300,
/* Configures how often service list notifications can occur. */
	SI468X_PROP_DAB_EVENT_MIN_SVRLIST_PERIOD = 0xB301,
/* Configures how often service list notifications can occur during reconfiguration events. */
	SI468X_PROP_DAB_EVENT_MIN_SVRLIST_PERIOD_RECONFIG = 0xB302,
/* Configures how often frequency information notifications can occur. */
	SI468X_PROP_DAB_EVENT_MIN_FREQINFO_PERIOD = 0xB303,
/* Selects which XPAD data will forwarded to the host. */
	SI468X_PROP_DAB_XPAD_ENABLE = 0xB400,
/* DAB_DRC_OPTION defines option to apply DRC (dynamic range control) gain. */
	SI468X_PROP_DAB_DRC_OPTION = 0xB401,
/* enables the feature of soft mute and comfort noise when signal level is low. */
	SI468X_PROP_DAB_ACF_ENABLE = 0xB500,
/* Set the signal RSSI threshold to mute audio. RSSI below this threshold indicates that signal is lost. In this case, audio will be muted */
	SI468X_PROP_DAB_ACF_MUTE_SIGLOSS_THRESHOLD = 0xB501,
/* Sets the BER limits when softmute engages. */
	SI468X_PROP_DAB_ACF_SOFTMUTE_BER_LIMITS = 0xB503,
/* Sets audio attenuation level. */
	SI468X_PROP_DAB_ACF_SOFTMUTE_ATTENUATION_LEVEL = 0xB504,
/* Sets mute time in ms. */
	SI468X_PROP_DAB_ACF_SOFTMUTE_ATTACK_TIME = 0xB505,
/* Sets unmute time in ms. */
	SI468X_PROP_DAB_ACF_SOFTMUTE_RELEASE_TIME = 0xB506,
/* Sets the BER limits when comfort noise engages. */
	SI468X_PROP_DAB_ACF_CMFTNOISE_BER_LIMITS = 0xB507,
/* Sets the comfort noise level. */
	SI468X_PROP_DAB_ACF_CMFTNOISE_LEVEL = 0xB508,
/* Sets comfort noise attack time in ms. */
	SI468X_PROP_DAB_ACF_CMFTNOISE_ATTACK_TIME = 0xB509,
/* Sets comfort noise release time in ms. */
	SI468X_PROP_DAB_ACF_CMFTNOISE_RELEASE_TIME = 0xB50A,
/* commontext */
	SI468X_PROP_DAB_ANNOUNCEMENT_ENABLE = 0xB700,
/* Sets up and enables the DAB BER test */
	SI468X_PROP_DAB_TEST_BER_CONFIG = 0xE800,
};

enum si468x_prop_pin_config_enable {
	SI468X_PROP_DAC_ENABLED	= 0x8001,
	SI468X_PROP_I2S_ENABLED	= 0x8002,
};

enum si468x_prop_fm_rds_config_bits {
	SI468X_PROP_RDSEN_MASK	= BIT(0),
	SI468X_PROP_RDSEN	= BIT(0),
};

enum si468x_prop_dab_event_interrupt_source_config_bits {
	SI468X_PROP_SRVLIST_INTEN_MASK		= BIT(0),
	SI468X_PROP_SRVLIST_INTEN		= BIT(0),
	SI468X_PROP_FREQINFO_INTEN_MASK		= BIT(1),
	SI468X_PROP_FREQINFO_INTEN_INTEN	= BIT(1),
	SI468X_PROP_SERVLINK_INTEN_MASK		= BIT(2),
	SI468X_PROP_SERVLINK_INTEN_INTEN	= BIT(2),
	SI468X_PROP_OESERV_INTEN_MASK		= BIT(3),
	SI468X_PROP_OESERV_INTEN_INTEN		= BIT(3),
	SI468X_PROP_ANNO_INTEN_MASK		= BIT(4),
	SI468X_PROP_ANNO_INTEN_INTEN		= BIT(4),
	SI468X_PROP_RECFGWRN_INTEN_MASK		= BIT(6),
	SI468X_PROP_RECFGWRN_INTEN		= BIT(6),
	SI468X_PROP_RECFG_INTEN_MASK		= BIT(7),
	SI468X_PROP_RECFG_INTEN_INTEN		= BIT(7),
};

enum si468x_prop_dab_service_interrupt_source_config_bits {
	SI468X_PROP_DSRV_INTEN_MASK		= BIT(0) | BIT(1),
	SI468X_PROP_DSRVOVFLINT_INTEN		= BIT(0),
	SI468X_PROP_DSRVPCKTINT_INTEN		= BIT(1),
};

int devm_regmap_init_si468x(struct si468x_core *);

#endif	/* SI468X_CORE_H */

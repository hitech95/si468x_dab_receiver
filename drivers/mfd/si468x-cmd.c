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

#include <linux/module.h>
#include <linux/completion.h>
#include <linux/crc32.h>
#include <linux/interrupt.h>
#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/atomic.h>
#include <linux/device.h>
#include <linux/gpio.h>
#include <linux/regulator/consumer.h>
#include <linux/firmware.h>
#include <linux/videodev2.h>

#include <linux/mfd/si468x-core.h>
#include "si468x-cmd_priv.h"

#include <asm/unaligned.h>

static LIST_HEAD(si468x_dab_channel_list);

/**
 * si468x_core_get_and_signal_status() - IRQ dispatcher
 * @core: Core device structure
 *
 * Dispatch the arrived interrupt request based on the value of the
 * status byte reported by the tuner.
 *
 */
void si468x_core_get_and_signal_status(struct si468x_core *core)
{
	int err;
	char command = CMD_RD_REPLY;
	char response[2];
	/* NRESP is at least 4 -> always update Power State */
	err = core->bus_ops->write(core, &command, sizeof(command));
	if (err < 0) {
		dev_err(core->dev, "Failed to send CMD_RD_REPLY %x\n", err);
		return;
	}
	err = core->bus_ops->read(core, response, sizeof(response));
	if (err < 0) {
		dev_err(core->dev, "Failed to get and signal status %x\n", err);
		return;
	}
	if (response[0] & SI468X_CTS) {
		/* Unfortunately completions could not be used for
		 * signalling CTS since this flag cannot be cleared
		 * in status byte, and therefore once it becomes true
		 * multiple calls to 'complete' would cause the
		 * commands following the current one to be completed
		 * before they actually are */
		dev_dbg(core->dev, "[interrupt] CTSINT\n");
		atomic_set(&core->cts, 1);
		wake_up(&core->command);
	}

	if (response[0] & SI468X_FM_RDS_INT) {
		dev_dbg(core->dev, "[interrupt] RDSINT\n");
		si468x_core_start_rds_drainer_once(core);
	}

	if (response[0] & SI468X_DACQ_INT) {
		/* Digital radio link change interrupt indicator.
		 * Indicates that something in the digital radio ensemble
		 * acquisition status has changed.
		 * Service via the DAB_DIGRAD_STATUS commands */
		dev_dbg(core->dev, "[interrupt] DACQ_INT\n");
	}

	if (response[0] & SI468X_DSRV_INT) {
		/* Indicates that an enabled data component of one
		 * of the digital services requires attention.
		 * Service using the GET_DIGITAL_SERVICE_DATA command */
		dev_dbg(core->dev, "[interrupt] DSRV_INT\n");
		si468x_core_get_digital_service_data(core);
	}

	if (response[1] & SI468X_DEVNT_INT) {
		/* Digital radio event change interrupt indicator.
		 * Indicates that a new event related to the digital radio
		 * has occurred. Service via the DAB_DIGRAD_STATUS commands */
		dev_dbg(core->dev, "[interrupt] DEVNT_INT\n");
		si468x_core_get_digital_service_list(core);
	}

	if (response[0] & SI468X_STC_INT) {
		dev_dbg(core->dev, "[interrupt] STCINT\n");
		atomic_set(&core->stc, 1);
		wake_up(&core->tuning);
	}
}

static irqreturn_t si468x_core_interrupt(int irq, void *dev)
{
	struct si468x_core *core = dev;

	si468x_core_get_and_signal_status(core);

	return IRQ_HANDLED;
}

static int si468x_core_parse_and_nag_about_error(struct si468x_core *core, u8 *buffer)
{
	int err;
	char *cause;

	if (buffer[4] & SI468X_RFFE_ERR)
		dev_err(core->dev,
		"The RF front end of the system is in an unexpected state\n");
	if (buffer[4] & SI468X_DSPERR)
		dev_err(core->dev,
		"The DSP has encountered a frame overrun. This is a fatal error\n");
	if (buffer[4] & SI468X_REPOFERR)
		dev_err(core->dev,
		"Control interface has dropped data during a reply read\n");
	if (buffer[4] & SI468X_CMDOFERR)
		dev_err(core->dev,
		"Control interface has dropped data during a command write\n");
	if (buffer[4] & SI468X_ARBERR)
		dev_err(core->dev,
		"An arbiter error has occurred\n");
	if (buffer[4] & SI468X_ERRNR)
		dev_err(core->dev,
		"A non-recoverable error has occurred\n");
	switch (buffer[4]) {
	case SI468X_ERR_UNSPECIFIED:
		cause = "Unspecified";
		err = -EINVAL;
		break;
	case SI468X_ERR_REPLY_OVERFLOW:
		cause = "Reply overflow";
		err = -EINVAL;
		break;
	case SI468X_ERR_NOT_AVAILABLE:
		cause = "Not available";
		err = -EINVAL;
		break;
	case SI468X_ERR_NOT_SUPPORTED:
		cause = "Not supported";
		err = -EINVAL;
		break;
	case SI468X_ERR_BAD_FREQUENCY:
		cause = "Bad frequency";
		err = -EINVAL;
		break;
	case SI468X_ERR_COMMAND_NOT_FOUND:
		cause = "Bad command";
		err = -EINVAL;
		break;
	case SI468X_ERR_BAD_ARG1:
		cause = "Bad argument #1";
		err = -EINVAL;
		break;
	case SI468X_ERR_BAD_ARG2:
		cause = "Bad argument #2";
		err = -EINVAL;
		break;
	case SI468X_ERR_BAD_ARG3:
		cause = "Bad argument #3";
		err = -EINVAL;
		break;
	case SI468X_ERR_BAD_ARG4:
		cause = "Bad argument #4";
		err = -EINVAL;
		break;
	case SI468X_ERR_BAD_ARG5:
		cause = "Bad argument #5";
		err = -EINVAL;
		break;
	case SI468X_ERR_BAD_ARG6:
		cause = "Bad argument #6";
		err = -EINVAL;
		break;
	case SI468X_ERR_BAD_ARG7:
		cause = "Bad argument #7";
		err = -EINVAL;
		break;
	case SI468X_ERR_COMMAND_BUSY:
		cause = "Command busy";
		err = -EBUSY;
		break;
	case SI468X_ERR_AT_BAND_LIMIT:
		cause = "At band limit or cannot further seek";
		err = -EINVAL;
		break;
	case SI468X_ERR_BAD_NVM:
		cause = "Bad NVM";
		err = -EIO;
		break;
	case SI468X_ERR_BAD_PATCH:
		cause = "Bad patch";
		err = -EINVAL;
		break;
	case SI468X_ERR_BAD_BOOT_MODE:
		cause = "Bad bootmode";
		err = -EINVAL;
		break;
	case SI468X_ERR_BAD_PROPERTY:
		cause = "Bad property";
		err = -EINVAL;
		break;
	case SI468X_ERR_NOT_ACQUIRED:
		cause = "Not acquired";
		err = -EINVAL;
		break;
	case SI468X_ERR_APP_NOT_SUPPORTED:
		cause = "APP not supported";
		err = -EINVAL;
		break;
	default:
		cause = "Unknown";
		err = -EIO;
	}

	dev_err(core->dev,
		"[Chip error status]: %s\n", cause);

	return err;
}

/**
 * si468x_core_send_command() - sends a command to si468x and waits its
 * response
 * @core:    si468x_device structure for the device we are
 *            communicating with
 * @command:  command id
 * @args:     command arguments we are sending
 * @argn:     actual size of @args
 * @response: buffer to place the expected response from the device
 * @respn:    actual size of @response
 * @usecs:    amount of time to wait before reading the response (in
 *            usecs)
 * @timeout:  report timeout
 *
 * Function returns 0 on succsess and negative error code on
 * failure
 */
static int si468x_core_send_command(struct si468x_core *core,
				    const u8 command,
				    const u8 args[],
				    const int argn,
				    u8 resp[],
				    const int respn,
				    const int usecs)
{
	int err;
	char cmd_rd_reply = CMD_RD_REPLY;
	u8  data[CMD_MAX_ARGS_COUNT + 1 + SI468X_MAX_HOST_LOAD_BYTES];

	if (core->power_state == SI468X_STATE_POWER_DOWN)
		return -EIO;

	if (argn > CMD_MAX_ARGS_COUNT + 1 + SI468X_MAX_HOST_LOAD_BYTES) {
		err = -ENOMEM;
		goto exit;
	}

	/* First send the command and its arguments */
	data[0] = command;
	memcpy(&data[1], args, argn);

	dev_dbg(core->dev, "Command:\n %*ph\n", argn + 1, data);
	err = core->bus_ops->write(core, (char *) data, argn + 1);
	if (err != argn + 1) {
		dev_err(core->dev,
			"Error while sending command 0x%02x\n",
			command);
		err = (err >= 0) ? -EIO : err;
		goto exit;
	}
	/* Set CTS to zero only after the command is send to avoid
	 * possible racing conditions */
	atomic_set(&core->cts, 0);

	/* if (unlikely(command == CMD_POWER_DOWN) */
	if (!wait_event_timeout(core->command,
				atomic_read(&core->cts),
				usecs_to_jiffies(usecs) + 1)) {
	/* chip does not respond with IRQ during power up and load */
		if (!((usecs == SI468X_TIMEOUT_POWER_UP) || (usecs == SI468X_TIMEOUT_LOAD)))
			dev_warn(core->dev,
				 "(%s) [CMD 0x%02x] Answer timeout.\n",
				 __func__, command);
		si468x_core_get_and_signal_status(core);
	}

	err = core->bus_ops->write(core, &cmd_rd_reply, sizeof(cmd_rd_reply));
	if (err < 0) {
		dev_err(core->dev, "Failed to send CMD_RD_REPLY %x\n", err);
		return err;
	}

	err = core->bus_ops->read(core, resp, respn);
	if (err < 0) {
		dev_err(core->dev, "Failed to get reply %x\n", err);
		return err;
	}

	switch (resp[3] & SI468X_PUP_MASK) {
	case SI468X_PUP_RESET:
		core->chip_state = SI468X_STATE_WAITING_FOR_POWER_UP_CMD;
		break;
	case SI468X_PUP_RES:
		core->chip_state = SI468X_STATE_RESERVED;
		break;
	case SI468X_PUP_BOOT:
		core->chip_state = SI468X_STATE_BOOTLOADER_RUNNING;
		break;
	case SI468X_PUP_APP:
		core->chip_state = SI468X_STATE_APPLICATION_RUNNING;
		break;
	}

	if (resp[0] & SI468X_ERR) {
		dev_err(core->dev,
			"[CMD 0x%02x] Chip set error flag\n", command);
		err = si468x_core_parse_and_nag_about_error(core, resp);
		goto exit;
	}

	if (!(resp[0] & SI468X_CTS))
		err = -EBUSY;
exit:
	return err;
}

static int si468x_cmd_send_boot(struct si468x_core *core)
{
	int err;
	u8       resp[CMD_BOOT_NRESP];
	const u8 args[CMD_BOOT_NARGS] = {
		0x00,
	};

	if (core->chip_state != SI468X_STATE_BOOTLOADER_RUNNING) {
		dev_err(core->dev,
			"Boot failure - Bootloader not active\n");
		err = -ENOTSUPP;
	}

	err = si468x_core_send_command(core, CMD_BOOT,
				       args, ARRAY_SIZE(args),
				       resp, ARRAY_SIZE(resp),
				       SI468X_TIMEOUT_BOOT);
	/* 63ms at analog fm, 198ms at DAB */

	if (core->chip_state != SI468X_STATE_APPLICATION_RUNNING) {
		dev_err(core->dev,
			"Boot failure - Application not running\n");
		err = -ENODEV;
	}
	return err;
}

static int si468x_cmd_get_sys_state(struct si468x_core *core)
{
	int err;
	char   *cause;
	u8       resp[CMD_GET_SYS_STATE_NRESP];
	const u8 args[CMD_GET_SYS_STATE_NARGS] = {
		0x00,
	};

	if (core->chip_state != SI468X_STATE_APPLICATION_RUNNING) {
		dev_err(core->dev,
			"sys state - Application not running\n");
		err = -ENOTSUPP;
	}

	err = si468x_core_send_command(core, CMD_GET_SYS_STATE,
				       args, ARRAY_SIZE(args),
				       resp, ARRAY_SIZE(resp),
				       SI468X_DEFAULT_TIMEOUT);
	if (err < 0)
		return err;

	switch (resp[4]) {
	case 0:
		cause = "Bootloader is active";
		break;
	case 1:
		cause = "FMHD is active";
		break;
	case 2:
		cause = "DAB is active";
		break;
	case 3:
		cause = "TDMB or data only DAB image is active";
		break;
	case 4:
		cause = "FMHD Demod is active";
		break;
	case 5:
		cause = "AMHD is active";
		break;
	case 6:
		cause = "AMHD Demod is active";
		break;
	case 7:
		cause = "DAB Demod is active";
		break;
	case 16:
		cause = "RESERVED";
		break;
	default:
		cause = "Unknown";
		err = -EIO;
	}
	dev_info(core->dev,
		"System state: %s\n", cause);
	return err;
}

/**
 * si468x_cmd_get_func_info() - send 'FUNC_INFO' command to the device
 *
 * @core: device to send the command to
 * @info:  struct si468x_part_and_function_info to fill all the information
 *         returned by the command
 *
 * The command requests device data
 *
 * Function returns 0 on succsess and negative error code on
 * failure
 */
static int si468x_cmd_get_func_info(struct si468x_core *core,
				    struct si468x_part_and_function_info *info)
{
	int err;
	u8       resp[CMD_GET_FUNC_INFO_NRESP];
	const u8 args[CMD_GET_FUNC_INFO_NARGS] = {
		0x00,
	};

	if (core->chip_state != SI468X_STATE_APPLICATION_RUNNING) {
		dev_err(core->dev,
			"sys state - Application not running\n");
		err = -ENOTSUPP;
	}

	err = si468x_core_send_command(core, CMD_GET_FUNC_INFO,
				       args, ARRAY_SIZE(args),
				       resp, ARRAY_SIZE(resp),
				       SI468X_DEFAULT_TIMEOUT);
	if (err < 0)
		return err;

	info->firmware.major = resp[4];
	info->firmware.minor = resp[5];
	info->firmware.build = resp[6];
	info->svn_id = get_unaligned_le32(resp + 8);
	dev_info(core->dev, "Revision:%d.%d.%d\n", info->firmware.major,
						   info->firmware.minor,
						   info->firmware.build);
	if (resp[7] & 0x80)
		dev_info(core->dev, "no SVN ID\n");
	else
		dev_info(core->dev, "SVN ID:%d\n", info->svn_id);
	switch (resp[7] & 0x30) {
	case 0x00:
		dev_info(core->dev, "Revision numbers are valid\n");
		break;
	case 0x10:
		dev_info(core->dev, "The image was built from an SVN branch\n");
		break;
	case 0x20:
		dev_info(core->dev, "The image was built from the trunk\n");
		break;
	default:
		dev_info(core->dev, "Location invalid\n");
	}
	if (resp[7] & 0x02)
		dev_info(core->dev, "Image was built with mixed revisions\n");
	if (resp[7] & 0x01)
		dev_info(core->dev, "Image has local modifications\n");
	return err;
}

/**
 * si468x_cmd_get_part_info() - send 'PART_INFO' command to the device
 * @core: device to send the command to
 * @info:  struct si468x_part_and_function_info to fill all the information
 *         returned by the command
 *
 * The command requests device data
 *
 * Function returns 0 on succsess and negative error code on
 * failure
 */
static int si468x_cmd_get_part_info(struct si468x_core *core,
			      struct si468x_part_and_function_info *info)
{
	int err;
	u8  resp[CMD_GET_PART_INFO_NRESP];
	const u8 args[CMD_GET_PART_INFO_NARGS] = {
		0x00,
	};

	err = si468x_core_send_command(core, CMD_GET_PART_INFO,
				       args, ARRAY_SIZE(args),
				       resp, ARRAY_SIZE(resp),
				       SI468X_DEFAULT_TIMEOUT * 4);

	if (!(err < 0)) {
		info->chip_revision = resp[4];
		info->rom_id = resp[5];
		info->part_info = get_unaligned_le16(resp + 8);
		dev_info(core->dev, "SI%d CHIPREV 0x%02x ROMID 0x%02x found\n",
			 info->part_info, info->chip_revision, info->rom_id);
	}

	return err;
}

/**
 * si468x_cmd_set_nvm_parameters() - set NVM SPI speed, etc
 * @core: Core device structure
 *
 */
static int si468x_cmd_set_nvm_parameters(struct si468x_core *core)
{
	int err;
	u8  resp[CMD_FLASH_LOAD_NRESP];
	const u8 args[CMD_FLASH_LOAD_NARGS + 6 * 4] = { /* 6 additional properties */
		0x10, /* FLASH_SET_PROP_LIST sub command */
		0x00,
		0x00,
		lsb(0x0001), /* SPI_CLOCK_FREQ_KHZ property */
		msb(0x0001),
		lsb(25000),   /* 25000 kHz */
		msb(25000),
		lsb(0x0003), /* SPI_MODE property */
		msb(0x0003),
		lsb(3),      /* SPI master mode */
		msb(3),
		lsb(0x0101), /* READ_CMD property */
		msb(0x0101),
		lsb(3),      /* Flash read command */
		msb(3),
		lsb(0x0102), /* HIGH_SPEED_READ_CMD property */
		msb(0x0102),
		lsb(0x0b),   /* Flash fast or high speed read command */
		msb(0x0b),
		lsb(0x0103), /* HIGH_SPEED_MAX_READ_FREQ_MHz property */
		msb(0x0103),
		lsb(80),     /* Maximum SPI clock frequency in MHz for the */
		msb(80),     /* fast or high speed read command */
		lsb(0x0201), /* WRITE_CMD property */
		msb(0x0201),
		lsb(2),      /* Flash write command */
		msb(2),
		lsb(0x0202), /* ERASE_SECTOR_CMD property */
		msb(0x0202),
		lsb(0x20),   /* Flash erase sector command */
		msb(0x20),
		lsb(0x0204), /* ERASE_CHIP_CMD property */
		msb(0x0204),
		lsb(0xc7),   /* Flash bulk/entire chip erase command */
		msb(0xc7),
	};
	if (core->nvm_state != SI468X_STATE_NVM_UNINITIALIZED)
		return 0;

	err = si468x_core_send_command(core, CMD_FLASH_LOAD,
				       args, ARRAY_SIZE(args),
				       resp, ARRAY_SIZE(resp),
				       SI468X_TIMEOUT_LOAD);
	if (err == CMD_FLASH_LOAD_NRESP)
		core->nvm_state = SI468X_STATE_NVM_READY;

	return err;
}

/**
 * si468x_cmd_load_firmware() - load firmware from file or flash to chip
 * @core: Core device structure
 * @fw_name: Firmware Filename
 *
 */
static int si468x_cmd_load_firmware(struct si468x_core *core,
					const u8 *of_shortname,
					bool load_to)
{
	const struct firmware *fw_entry = NULL;
	u8  *fw_data = NULL;
	int err, err_flash, err_fw, fw_len = 0;
	const char *of_name, *fw_name;
	u32 flash_base_address = 0, flash_address, size, crc;
	u8       init_resp[CMD_LOAD_INIT_NRESP];
	const u8 init_args[CMD_LOAD_INIT_NARGS] = {
		0x00,
	};
	u8 load_resp[CMD_HOST_LOAD_NRESP];
	u8 flash_resp[CMD_FLASH_LOAD_NRESP];
	u8 args[max_t(int, CMD_FLASH_LOAD_NARGS + 5 + SI468X_MAX_HOST_LOAD_BYTES,
			   CMD_HOST_LOAD_NARGS + 1 + SI468X_MAX_HOST_LOAD_BYTES)];

	of_name = kasprintf(GFP_KERNEL, "flash-%s", of_shortname);
	if (!of_name)
		return -ENOMEM;
	err_flash = device_property_read_u32(core->dev, of_name, &flash_base_address);
	kfree(of_name);
	flash_address = flash_base_address;

	of_name = kasprintf(GFP_KERNEL, "firmware-%s", of_shortname);
	if (!of_name)
		return -ENOMEM;
	err_fw = device_property_read_string(core->dev, of_name, &fw_name);
	kfree(of_name);

	if (err_flash && err_fw) {
		dev_err(core->dev, "No firmware found in dt\n");
		return -ENODATA;
	}
	
	/* prefer firmware over flash load (e.g. to test new firmware) */

	if (!err_fw) {
		err = request_firmware(&fw_entry, fw_name, core->dev);
		if (err < 0) {
			dev_err(core->dev,
				"Unable to read firmware(%s) content\n",
				fw_name);
			/* file does not exist and flash address not defined */
			if (err_flash < 0)
				return err;
		} else {
			dev_info(core->dev, "Firmware(%s) length : %zu bytes\n",
				 fw_name, fw_entry->size);
			fw_data = (void *)fw_entry->data;
			fw_len = fw_entry->size;
		}
	}

	if (load_to == SI468X_LOAD_TO_HOST) {
		err = si468x_core_send_command(core, CMD_LOAD_INIT,
			       init_args, ARRAY_SIZE(init_args),
			       init_resp, ARRAY_SIZE(init_resp),
			       SI468X_TIMEOUT_LOAD);
		if (err < 0)
			goto exit;
	}

	if ((load_to == SI468X_LOAD_TO_FLASH) || (!fw_entry)) {
		err = si468x_cmd_set_nvm_parameters(core);
		if (err < 0)
			goto exit;
	}

	if (fw_entry) {
		while (fw_data && fw_len > 0) {
			size = min_t(int, fw_len, SI468X_MAX_HOST_LOAD_BYTES);
			memset(args, 0, ARRAY_SIZE(args));
			if (load_to == SI468X_LOAD_TO_HOST) {
				memcpy(args + CMD_HOST_LOAD_NARGS, fw_data, size);
				dev_dbg_ratelimited(core->dev, "HOST load: %*ph\n",
						     CMD_HOST_LOAD_NARGS + 4, args);
				err = si468x_core_send_command(core, CMD_HOST_LOAD,
					       args, CMD_HOST_LOAD_NARGS + size,
					       load_resp, ARRAY_SIZE(load_resp),
					       SI468X_TIMEOUT_LOAD);
			} else {
				memcpy(args + CMD_FLASH_LOAD_NARGS + 4, fw_data, size); /* SUBCMD1..3 */
				args[0] = 0xf0;
				args[1] = 0x0c;
				args[2] = 0xed;
				args[3] = 0x00;
				args[4] = 0x00;
				args[5] = 0x00;
				args[6] = 0x00;
				args[7] = cpu_to_le32(flash_address) & 0xff;
				args[8] = (cpu_to_le32(flash_address) >> 8) & 0xff;
				args[9] = (cpu_to_le32(flash_address) >> 16) & 0xff;
				args[10] = (cpu_to_le32(flash_address) >> 24) & 0xff;
				args[11] = cpu_to_le32(size) & 0xff;
				args[12] = (cpu_to_le32(size) >> 8) & 0xff;
				args[13] = (cpu_to_le32(size) >> 16) & 0xff;
				args[14] = (cpu_to_le32(size) >> 24) & 0xff;
				dev_dbg_ratelimited(core->dev, "FLASH load: %*ph\n",
						     CMD_FLASH_LOAD_NARGS + 4, args);
				err = si468x_core_send_command(core, CMD_FLASH_LOAD,
					       args, CMD_FLASH_LOAD_NARGS + 4 + size,
					       flash_resp, ARRAY_SIZE(flash_resp),
					       SI468X_DEFAULT_TIMEOUT);
				flash_address += SI468X_MAX_HOST_LOAD_BYTES;
			}
			if (err < 0)
				goto exit;

			fw_data += SI468X_MAX_HOST_LOAD_BYTES;
			fw_len -= SI468X_MAX_HOST_LOAD_BYTES;
		}
		if (load_to == SI468X_LOAD_TO_FLASH) {
			crc = crc32_be(0xFFFFFFFF, fw_entry->data, fw_entry->size);
			args[0] = 0x02;
			args[1] = 0x00;
			args[2] = 0x00;
			args[3] = cpu_to_le32(crc) & 0xff;
			args[4] = (cpu_to_le32(crc) >> 8) & 0xff;
			args[5] = (cpu_to_le32(crc) >> 16) & 0xff;
			args[6] = (cpu_to_le32(crc) >> 24) & 0xff;
			args[7] = cpu_to_le32(flash_base_address) & 0xff;
			args[8] = (cpu_to_le32(flash_base_address) >> 8) & 0xff;
			args[9] = (cpu_to_le32(flash_base_address) >> 16) & 0xff;
			args[10] = (cpu_to_le32(flash_base_address) >> 24) & 0xff;
			args[11] = cpu_to_le32(fw_entry->size) & 0xff;
			args[12] = (cpu_to_le32(fw_entry->size) >> 8) & 0xff;
			args[13] = (cpu_to_le32(fw_entry->size) >> 16) & 0xff;
			args[14] = (cpu_to_le32(fw_entry->size) >> 24) & 0xff;
			err = si468x_core_send_command(core, CMD_FLASH_LOAD,
					args, CMD_FLASH_LOAD_NARGS + 4,
					flash_resp, ARRAY_SIZE(flash_resp),
					SI468X_TIMEOUT_CRC);
			if (err < 0) {
				dev_err(core->dev, "FLASH load: CRC Error 0x%08x\n", err);
				goto exit;
			}
		}
	} else {
		if (!err_flash) {
			memset(args, 0, ARRAY_SIZE(args));
			args[3] = cpu_to_le32(flash_base_address) & 0xff;
			args[4] = (cpu_to_le32(flash_base_address) >> 8) & 0xff;
			args[5] = (cpu_to_le32(flash_base_address) >> 16) & 0xff;
			args[6] = (cpu_to_le32(flash_base_address) >> 24) & 0xff;
			dev_info(core->dev, "FLASH load: %s\n", of_shortname);
			err = si468x_core_send_command(core, CMD_FLASH_LOAD,
				       args, CMD_FLASH_LOAD_NARGS,
				       flash_resp, ARRAY_SIZE(flash_resp),
				       SI468X_DEFAULT_TIMEOUT * 4);
		}
	}
exit:
	release_firmware(fw_entry);
	return err;
}

/**
 * si468x_core_select_func() - boot chip with propper firmware
 * @core: Core device structure
 * @func: selects firmware to load
 *
 * The function returns zero in case of success or negative error code
 * otherwise.
 */
int si468x_core_select_func(struct si468x_core *core, enum si468x_func func)
{
	int err = 0;
	int irq_map;
	struct si468x_part_and_function_info info;

	if (func == SI468X_FUNC_MINI_BOOT)
		return 0;

	err = si468x_cmd_load_firmware(core, "patch", SI468X_LOAD_TO_HOST);
	if (err < 0) {
		dev_err(core->dev,
			"Failed to download patch"
			"(err = %d)\n", err);
		return -EIO;
	}
	msleep(4); /* see flowcharts in AN649 Ref 2.0 */

	if (func == SI468X_FUNC_BOOTLOADER) {
		atomic_set(&core->is_alive, 1);
		return 0;
	}

	err = si468x_cmd_load_firmware(core, si468x_func_string_table[func],
				       SI468X_LOAD_TO_HOST);
	if (err < 0) {
		dev_err(core->dev,
			"Failed to download %s"
			"(err = %d)\n", si468x_func_string_table[func], err);
		return -EIO;
	}

	err = si468x_cmd_send_boot(core);
	if (err < 0)
		return -EIO;

	err = si468x_cmd_get_sys_state(core);
	if (err < 0)
		return -EIO;

	err = si468x_cmd_get_func_info(core, &info);
	if (err < 0)
		return -EIO;

	atomic_set(&core->is_alive, 1);

	irq_map = SI468X_STCIEN | SI468X_CTSIEN;
	if (func == SI468X_FUNC_FM_RECEIVER)
		irq_map |= SI468X_RDSIEN;
	if (func == SI468X_FUNC_DAB_RECEIVER)
		irq_map |= SI468X_DEVNTIEN | SI468X_DSRVIEN;
	err = regmap_write(core->regmap_common,
			   SI468X_PROP_INT_CTL_ENABLE,
			   irq_map);
	if (err < 0) {
		dev_err(core->dev,
			"Failed to configure interrupt sources"
			"(err = %d)\n", err);
		return -EIO;
	}
	return 0;
}
EXPORT_SYMBOL_GPL(si468x_core_select_func);

/**
 * si468x_core_start() - early chip startup function
 * @core: Core device structure
 * power down is the one done by sending appropriate command instead
 * of using reset pin of the tuner
 *
 * Perform required startup sequence to correctly power
 * up the chip and perform initial configuration. It does the
 * following sequence of actions:
 *       1. Claims and enables the power supplies VD and VIO1 required
 *          for I2C or SPI interface of the chip operation.
 *       2. Waits for 10us, pulls the reset line up, enables irq,
 *          waits for another 5000us as it is specified by the
 *          datasheet.
 *       3. Sends 'POWER_UP' command to the device with all provided
 *          information about power-up parameters.
 *       4. Configures, enables digital audio and
 *          configures interrupt sources.
 *
 * The function returns zero in case of success or negative error code
 * otherwise.
 */
int si468x_core_start(struct si468x_core *core)
{
	int    err = 0;
	u8       power_up_resp[CMD_POWER_UP_NRESP];
	const u8 power_up_args[CMD_POWER_UP_NARGS] = {
		SI468X_CTSIEN,
		((core->power_up_parameters.xmode << 4) & 0x30) | ((core->power_up_parameters.tr_size) & 0x0f),
		(core->power_up_parameters.ibias) & 0x7f,
		(core->power_up_parameters.clk_frequency) & 0xFF,
		((core->power_up_parameters.clk_frequency) >> 8) & 0xFF,
		((core->power_up_parameters.clk_frequency) >> 16) & 0xFF,
		((core->power_up_parameters.clk_frequency) >> 24) & 0xFF,
		(core->power_up_parameters.xcload) & 0x7f,
		0x10,
		0x00,
		0x00,
		0x00,
		((core->power_up_parameters.ibias) / 2) & 0x7f,
		0x00,
		0x00,
	};

	gpiod_set_value_cansleep(core->gpio_reset, 0);

	msleep(3); /* RSTB rise to start of POWER_UP Command */

	enable_irq(core->irq);

	err = si468x_core_send_command(core, CMD_POWER_UP,
				       power_up_args, ARRAY_SIZE(power_up_args),
				       power_up_resp, ARRAY_SIZE(power_up_resp),
				       SI468X_TIMEOUT_POWER_UP);
	/* POWER_UP does not set IRQ! Check with RD_REPLY */
	if (err < 0)
		return err;

	if (core->chip_state != SI468X_STATE_BOOTLOADER_RUNNING) {
		dev_err(core->dev,
			"Power up failure - Bootloader not running\n");
		err = -ENODEV;
		goto disable_irq;
	}

	err = si468x_cmd_load_firmware(core,
				       si468x_func_string_table[SI468X_FUNC_MINI_BOOT],
				       SI468X_LOAD_TO_HOST);
	if (err < 0) {
		dev_err(core->dev,
			"Failed to download mini patch"
			"(err = %d)\n", err);
		goto disable_irq;
	}
	msleep(4); /* see flowcharts in AN649 Ref 2.0 */

	err = si468x_core_select_func(core, core->power_up_parameters.func);
	if (err < 0) {
		dev_err(core->dev,
			"Failed to select function"
			"(err = %d)\n", err);
		goto disable_irq;
	}

	atomic_set(&core->is_alive, 1);

	return 0;

disable_irq:
	if (err == -ENODEV)
		atomic_set(&core->is_alive, 0);

	disable_irq(core->irq);

	gpiod_set_value_cansleep(core->gpio_reset, 1);

	return err;
}
EXPORT_SYMBOL_GPL(si468x_core_start);

/**
 * si468x_core_stop() - chip power-down function
 * @core: Core device structure
 * bringing reset line low
 *
 * Power down the chip by performing following actions:
 * 1. Disable IRQ
 * 2. bring reset line active.
 *
 * The function returns zero in case of success or negative error code
 * otherwise.
 */
void si468x_core_stop(struct si468x_core *core)
{
	regcache_cache_only(core->regmap_common, true);
	regcache_cache_only(core->regmap_fm, true);
	if (core->si468x_device_info->has_am)
		regcache_cache_only(core->regmap_am, true);
	if (core->si468x_device_info->has_dab)
		regcache_cache_only(core->regmap_dab, true);

	atomic_set(&core->is_alive, 0);

	disable_irq(core->irq);

	gpiod_set_value_cansleep(core->gpio_reset, 1);
	return;
}
EXPORT_SYMBOL_GPL(si468x_core_stop);

/**
 * si468x_core_pronounce_dead()
 *
 * @core: Core device structure
 *
 * Mark the device as being dead and wake up all potentially waiting
 * threads of execution.
 *
 */
void si468x_core_pronounce_dead(struct si468x_core *core)
{
	dev_info(core->dev, "Core device is dead.\n");

	atomic_set(&core->is_alive, 0);

	/* Wake up al possible waiting processes */
	wake_up_interruptible(&core->rds_read_queue);

	atomic_set(&core->cts, 1);
	wake_up(&core->command);

	atomic_set(&core->stc, 1);
	wake_up(&core->tuning);
}
EXPORT_SYMBOL_GPL(si468x_core_pronounce_dead);

/**
 * si473x_core_set_power_state() - set the desired chip state.
 * @core: Core device structure
 * @next_state: enum si468x_power_state describing state to switch to.
 *
 * Switch on all the required power supplies
 *
 * This function returns 0 in case of suvccess and negative error code
 * otherwise.
 */
int si473x_core_set_power_state(struct si468x_core *core,
				enum si468x_power_state next_state)
{
	/*
	   It is not clear form the datasheet if it is possible to
	   work with device if not all power domains are operational.
	   So for now the power-up policy is "power-up all the things!"
	 */
	int err = 0;

	if (core->power_state == SI468X_STATE_POWER_INCONSISTENT) {
		dev_err(core->dev,
			"The device in inconsistent power state\n");
		return -EINVAL;
	}

	if (next_state != core->power_state) {
		switch (next_state) {
		case SI468X_STATE_POWER_UP:
			err = regulator_bulk_enable(ARRAY_SIZE(core->supplies),
						    core->supplies);
			if (err < 0) {
				core->power_state = SI468X_STATE_POWER_INCONSISTENT;
				break;
			}
			/*
			 * Startup timing diagram recommends to have a
			 * 10 us delay between enabling of the power
			 * supplies and turning the tuner on.
			 */
			udelay(10);

			core->nvm_state = SI468X_STATE_NVM_UNINITIALIZED;
			core->chip_state = SI468X_STATE_WAITING_FOR_POWER_UP_CMD;
			core->power_state = next_state;
			err = si468x_core_start(core);
			if (err < 0)
				goto disable_regulators;

			break;

		case SI468X_STATE_POWER_DOWN:
			core->power_state = next_state;
			si468x_core_stop(core);
disable_regulators:
			err = regulator_bulk_disable(ARRAY_SIZE(core->supplies),
						     core->supplies);
			if (err < 0)
				core->power_state = SI468X_STATE_POWER_INCONSISTENT;
			break;
		default:
			break;
		}
	}

	return err;
}
EXPORT_SYMBOL_GPL(si473x_core_set_power_state);

/**
 * si468x_core_get_revision_info()
 * @core: Core device structure
 *
 * Get the firmware version number of the device. It is done in
 * following three steps:
 *    1. Power-up the device
 *    2. Send the 'FUNC_INFO' command
 *    3. Powering the device down.
 *
 * The function return zero on success and a negative error code on
 * failure.
 */
int si468x_core_get_revision_info(struct si468x_core *core)
{
	int rval;
	struct si468x_part_and_function_info info;

	info.part_info = 0;
	core->power_up_parameters.func = SI468X_FUNC_MINI_BOOT;
	si468x_core_lock(core);
	rval = si473x_core_set_power_state(core, SI468X_STATE_POWER_UP);
	if (rval < 0)
		goto exit;

	rval = si468x_cmd_get_part_info(core, &info);

	si473x_core_set_power_state(core, SI468X_STATE_POWER_DOWN);
exit:
	si468x_core_unlock(core);

	if (core->si468x_device_info->device_id != info.part_info) {
		rval = -EINVAL;
		dev_err(core->dev,
			"dt device id SI%d does not match chips id (SI%d)\n",
			core->si468x_device_info->device_id,
			info.part_info);
	}

	return rval;
}

bool si468x_core_is_in_am_receiver_mode(struct si468x_core *core)
{
	return core->si468x_device_info->has_am &&
		(core->power_up_parameters.func == SI468X_FUNC_AM_RECEIVER);
}
EXPORT_SYMBOL_GPL(si468x_core_is_in_am_receiver_mode);

bool si468x_core_is_in_fm_receiver_mode(struct si468x_core *core)
{
	return (core->power_up_parameters.func == SI468X_FUNC_FM_RECEIVER);
}
EXPORT_SYMBOL_GPL(si468x_core_is_in_fm_receiver_mode);

bool si468x_core_is_in_dab_receiver_mode(struct si468x_core *core)
{
	return core->si468x_device_info->has_dab &&
		(core->power_up_parameters.func == SI468X_FUNC_DAB_RECEIVER);
}
EXPORT_SYMBOL_GPL(si468x_core_is_in_dab_receiver_mode);

/**
 * si468x_core_report_drainer_stop() - mark the completion of the RDS
 * buffer drain porcess by the worker.
 *
 * @core: Core device structure
 */
static inline void si468x_core_report_drainer_stop(struct si468x_core *core)
{
	mutex_lock(&core->rds_drainer_status_lock);
	core->rds_drainer_is_working = false;
	mutex_unlock(&core->rds_drainer_status_lock);
}

/**
 * si468x_core_start_rds_drainer_once() - start RDS drainer worker if
 * there is none working, do nothing otherwise
 *
 * @core: Datastructure corresponding to the chip.
 */
static inline void si468x_core_start_rds_drainer_once(struct si468x_core *core)
{
	mutex_lock(&core->rds_drainer_status_lock);
	if (!core->rds_drainer_is_working) {
		core->rds_drainer_is_working = true;
		schedule_work(&core->rds_fifo_drainer);
	}
	mutex_unlock(&core->rds_drainer_status_lock);
}

/**
 * si468x_drain_rds_fifo() - RDS buffer drainer.
 * @work: struct work_struct being passed to the function by the
 * kernel.
 *
 * Drain the contents of the RDS FIFO of
 */
void si468x_core_drain_rds_fifo(struct work_struct *work)
{
	int err;

	struct si468x_core *core = container_of(work, struct si468x_core,
						rds_fifo_drainer);

	struct si468x_rds_status_report report;

	si468x_core_lock(core);

	err = si468x_core_cmd_fm_rds_status(core, true, false, false, &report);
	if (!(err < 0)) {
		int i = report.rdsfifoused;
		dev_dbg(core->dev,
			"%d elements in RDS FIFO. Draining.\n", i);
		for (; i > 0; --i) {
			err = si468x_core_cmd_fm_rds_status(core, false, false,
							    (i == 1), &report);
			if (err < 0)
				goto unlock;

			kfifo_in(&core->rds_fifo, report.rds,
				 sizeof(report.rds));
			dev_dbg(core->dev, "RDS data:\n %*ph\n",
				(int)sizeof(report.rds), report.rds);
		}
		dev_dbg(core->dev, "Drrrrained!\n");
		wake_up_interruptible(&core->rds_read_queue);
	}

unlock:
	si468x_core_unlock(core);
	si468x_core_report_drainer_stop(core);
}

int si468x_core_cmd_dab_start_service(struct si468x_core *core,
				      struct si468x_dab_channel *channel)
{
	struct si468x_rsq_status_args rsq_args = {
		.rsqack		= false,
		.digradack	= false,
		.attune		= false,
		.cancel		= false,
		.fiberrack	= false,
		.stcack		= false,
	};
	struct si468x_rsq_status_report rsq_report;
	struct si468x_tune_freq_args tune_args = {
		.injside	= SI468X_INJSIDE_AUTO,
		.antcap		= 0,
		.direct_tune	= SI468X_SELECT_MAIN_PROGRAM_SERVICE,
		.program_id	= 0,
	};

	int err;
	u8 resp[CMD_START_DIGITAL_SERVICE_NRESP];
	u8 args[CMD_START_DIGITAL_SERVICE_NARGS] = {
		0, /* For DAB/DMB applications this parameter should be 0 */
		0,
		0,
		(channel->service_id) & 0xFF,
		((channel->service_id) >> 8) & 0xFF,
		((channel->service_id) >> 16) & 0xFF,
		((channel->service_id) >> 24) & 0xFF,
		((channel->component_info.sub_ch_id) |
		 (channel->component_info.fidc_id) |
		 (channel->component_info.sc_id))  & 0xFF,
		((channel->component_info.tm_id << 6) |
		 (channel->component_info.dg_flag << 5) |
		 (channel->component_info.sc_id >> 8))& 0xFF,
		(channel->component_info.audio_service_type << 2) |
		(channel->component_info.data_service_type << 2) |
		(channel->component_info.is_secondary << 1) |
		(channel->component_info.access_control_flag),
		(channel->component_info.mua_info_valid),
	};

	if (channel->is_audio_service)
		args[4] |= channel->country_id << 4;
	if (channel->is_data_service) {
		args[5] |= channel->country_id << 4;
		args[6] = channel->extended_country_code;
	}

	err = si468x_core_cmd_dab_rsq_status(core, &rsq_args, &rsq_report);
	if (err < 0)
		return err;
	if (channel->frequency_index != rsq_report.tune_index) {
		tune_args.dab_freq_list = core->loaded_dab_freq_list;
		tune_args.freq = channel->frequency;
		err = si468x_core_cmd_dab_tune_freq(core, &tune_args);
		if (err < 0)
			return err;
	}

	err = si468x_core_send_command(core, CMD_START_DIGITAL_SERVICE,
				       args, ARRAY_SIZE(args),
				       resp, ARRAY_SIZE(resp),
				       SI468X_DEFAULT_TIMEOUT);
	if (err < 0)
		return err;
	channel->is_started = true;
	return err;
}
EXPORT_SYMBOL_GPL(si468x_core_cmd_dab_start_service);

int si468x_core_cmd_dab_stop_service(struct si468x_core *core,
				      struct si468x_dab_channel *channel)
{
	int err;
	u8 resp[CMD_STOP_DIGITAL_SERVICE_NRESP];
	u8 args[CMD_STOP_DIGITAL_SERVICE_NARGS] = {
		0, /* For DAB/DMB applicationsthis parameter should be 0 */
		0,
		0,
		(channel->service_id) & 0xFF,
		((channel->service_id) >> 8) & 0xFF,
		((channel->service_id) >> 16) & 0xFF,
		((channel->service_id) >> 24) & 0xFF,
		((channel->component_info.sub_ch_id) |
		 (channel->component_info.fidc_id) |
		 (channel->component_info.sc_id))  & 0xFF,
		((channel->component_info.tm_id << 6) |
		 (channel->component_info.dg_flag << 5) |
		 (channel->component_info.sc_id >> 8))& 0xFF,
		(channel->component_info.audio_service_type << 2) |
		(channel->component_info.data_service_type << 2) |
		(channel->component_info.is_secondary << 1) |
		(channel->component_info.access_control_flag),
		(channel->component_info.mua_info_valid),
	};
	if (channel->is_audio_service)
		args[4] |= channel->country_id << 4;
	if (channel->is_data_service) {
		args[5] |= channel->country_id << 4;
		args[6] = channel->extended_country_code;
	}

	err = si468x_core_send_command(core, CMD_STOP_DIGITAL_SERVICE,
				       args, ARRAY_SIZE(args),
				       resp, ARRAY_SIZE(resp),
				       SI468X_DEFAULT_TIMEOUT);
	if (err < 0)
		return err;
	channel->is_started = false;
	return err;
}
EXPORT_SYMBOL_GPL(si468x_core_cmd_dab_stop_service);

int si468x_core_cmd_dab_get_digital_service_data(struct si468x_core *core,
		bool status_only,
		bool intack,
		struct si468x_digital_service_data_status_report *report)
{
	int err;
	u8 *payload;
	char cmd_rd_reply = CMD_RD_REPLY;
	u8 resp[CMD_GET_DIGITAL_SERVICE_DATA_NRESP];
	u8 args[CMD_GET_DIGITAL_SERVICE_DATA_NARGS] = {
		status_only << 4 | intack,
	};
	err = si468x_core_send_command(core,
				       CMD_GET_DIGITAL_SERVICE_DATA,
				       args, ARRAY_SIZE(args),
				       resp, ARRAY_SIZE(resp),
				       SI468X_DEFAULT_TIMEOUT);
	if (err < 0)
		return err;
	report->dsrvovflint = 0x02 & resp[4];
	report->dsrvpcktint = 0x01 & resp[4];
	report->buff_count = resp[5];
	report->srv_state = resp[6];
	report->data_src = (0xc0 & resp[7]) >> 6;
	report->dscty = 0x3f & resp[7];
	report->service_id = get_unaligned_le32(resp + 8);
	report->comp_id = get_unaligned_le32(resp + 12);
	report->uatype = get_unaligned_le16(resp + 16);
	report->byte_count = get_unaligned_le16(resp + 18);
	report->seg_num = get_unaligned_le16(resp + 20);
	report->num_segs = get_unaligned_le16(resp + 22);

	if (status_only)
		return err;


	payload = kmalloc(report->byte_count + ARRAY_SIZE(resp),
			  GFP_KERNEL);
	if (!payload)
		return -ENOMEM;

	/* not beautiful, but issue RD_REPLY again to get payload */
	err = core->bus_ops->write(core, &cmd_rd_reply, sizeof(cmd_rd_reply));
	if (err < 0) {
		dev_err(core->dev, "Failed to send CMD_RD_REPLY %x\n", err);
		goto free_kmem;
	}

	err = core->bus_ops->read(core, payload,
				  report->byte_count + ARRAY_SIZE(resp));
	if (err < 0) {
		dev_err(core->dev, "Failed to get reply %x\n", err);
		goto free_kmem;
	}
	memcpy(report->payload, payload + ARRAY_SIZE(resp),
	       report->byte_count);
free_kmem:
	kfree(payload);

	return err;
}
EXPORT_SYMBOL_GPL(si468x_core_cmd_dab_get_digital_service_data);

/**
 * si468x_core_get_digital_service_list() - get new list worker if
 * there is none working, do nothing otherwise
 *
 * @core: Datastructure corresponding to the chip.
 */
static inline void si468x_core_get_digital_service_list(struct si468x_core *core)
{
	schedule_work(&core->update_service_list);
}

/**
 * si468x_core_new_digital_service_list() - updates service list.
 * @work: struct work_struct being passed to the function by the
 * kernel.
 */
void si468x_core_new_digital_service_list(struct work_struct *work)
{
	int err;
	int srvnr, compnr;
	u8  max_signal_strength;

	struct si468x_core *core = container_of(work, struct si468x_core,
						update_service_list);

	struct si468x_event_status_args eventargs;
	struct si468x_event_status_report report;
	struct si468x_rsq_status_args rsq_args = {
		.rsqack		= false,
		.digradack	= false,
		.attune		= false,
		.cancel		= false,
		.fiberrack	= false,
		.stcack		= false,
	};
	struct si468x_rsq_status_report rsq_report;
	struct si468x_dab_service_list *list;
	struct si468x_dab_channel *channel;
	struct si468x_dab_channel *ptr, *next;
	struct si468x_tune_freq_args args = {
		.injside	= SI468X_INJSIDE_AUTO,
		.antcap		= 0,
		.direct_tune	= SI468X_SELECT_MAIN_PROGRAM_SERVICE,
		.program_id	= 0,
	};

	eventargs.eventack = true;
	si468x_core_lock(core);
	err = si468x_core_cmd_dab_event_status(core, &eventargs, &report);
	if (err < 0)
		goto unlock;
	err = si468x_core_cmd_dab_rsq_status(core, &rsq_args, &rsq_report);
	if (err < 0)
		goto unlock;

	list = kzalloc(sizeof(struct si468x_dab_service_list),
		       GFP_KERNEL);
	if (!list)
		goto unlock;
	err = si468x_core_cmd_dab_get_service_list(core, list);
	if (err < 0) {
		goto free_kmem;
	}

	list_for_each_entry_safe(ptr, next, &si468x_dab_channel_list, list) {
		if (ptr->frequency_index == rsq_report.tune_index) {
			list_del(&ptr->list);
			kfree(ptr);
		}
	}

	for (srvnr = 0; /* save list */
	     srvnr < list->number_of_services;
	     srvnr++) {
		for (compnr = 0;
		     compnr < list->si468x_dab_service_info[srvnr].number_of_components;
		     compnr++) {
			if (list->si468x_dab_service_info[srvnr].is_data_service)
				continue;
			channel = kzalloc(sizeof(struct si468x_dab_channel), GFP_KERNEL);
			if (!channel)
				goto free_kmem;
			INIT_LIST_HEAD(&channel->list);
			list_add_tail(&channel->list, &si468x_dab_channel_list);
			channel->version = list->version;
			channel->frequency_index = rsq_report.tune_index;
			channel->frequency = rsq_report.readfreq;
			channel->fic_quality = rsq_report.fic_quality;
			channel->signal_strength = rsq_report.rssi;
			channel->service_id =
			list->si468x_dab_service_info[srvnr].service_id;
			channel->country_id =
			list->si468x_dab_service_info[srvnr].country_id;
			channel->is_data_service =
			list->si468x_dab_service_info[srvnr].is_data_service;
			channel->is_audio_service =
			list->si468x_dab_service_info[srvnr].is_audio_service;
			channel->component_info =
			list->si468x_dab_service_info[srvnr].si468x_dab_component_info[compnr];
			strscpy(channel->service_label,
				list->si468x_dab_service_info[srvnr].service_label,
				sizeof(list->si468x_dab_service_info[srvnr].service_label));
		}
	}

	if (atomic_read(&core->dab_full_scan)) {
		if (core->loaded_dab_freq_list[rsq_report.tune_index + 1].frequency) {
			args.dab_freq_list = core->loaded_dab_freq_list;
			args.freq = core->loaded_dab_freq_list[rsq_report.tune_index + 1].frequency;
			err = si468x_core_cmd_dab_tune_freq(core, &args);
			if (err < 0)
				goto free_kmem;
		} else {
			atomic_set(&core->dab_full_scan, 0);
			/* tune to max rssi, first audio service */
			list_for_each_entry(ptr, &si468x_dab_channel_list, list) {
				max_signal_strength = max(ptr->signal_strength,
							  max_signal_strength);
			}
			list_for_each_entry(ptr, &si468x_dab_channel_list, list) {
				if(ptr->signal_strength == max_signal_strength &&
				   ptr->is_audio_service)
					break;
			}
			if (ptr->frequency_index != rsq_report.tune_index) {
				args.dab_freq_list = core->loaded_dab_freq_list;
				args.freq = ptr->frequency;
				err = si468x_core_cmd_dab_tune_freq(core, &args);
				if (err < 0)
					goto free_kmem;
			}
			err = regmap_update_bits(core->regmap_common,
				SI468X_PROP_DIGITAL_SERVICE_INT_SOURCE,
				SI468X_PROP_DSRV_INTEN_MASK,
				SI468X_PROP_DSRVPCKTINT_INTEN |
				SI468X_PROP_DSRVOVFLINT_INTEN);
			if (err < 0)
				goto free_kmem;

			err = si468x_core_cmd_dab_start_service(core, ptr);
			if (err < 0)
				goto free_kmem;
		}
	}
free_kmem:
	kfree(list);
unlock:
	si468x_core_unlock(core);
}
/**
 * si468x_core_get_digital_service_data() - get new data worker if
 * there is none working, do nothing otherwise
 *
 * @core: Datastructure corresponding to the chip.
 */
static inline void si468x_core_get_digital_service_data(struct si468x_core *core)
{
	mutex_lock(&core->digital_service_drainer_status_lock);
	if (!core->digital_service_drainer_is_working) {
		core->digital_service_drainer_is_working = true;
		schedule_work(&core->update_service_data);
	}
	mutex_unlock(&core->digital_service_drainer_status_lock);
}

/**
 * si468x_core_new_digital_service_data() - updates service data.
 * @work: struct work_struct being passed to the function by the
 * kernel.
 */
void si468x_core_new_digital_service_data(struct work_struct *work)
{
	int err;
	struct si468x_core *core = container_of(work, struct si468x_core,
						update_service_data);
	struct si468x_digital_service_data_status_report report;

	si468x_core_lock(core);

	err = si468x_core_cmd_dab_get_digital_service_data(core, true, true, &report);
	if (err < 0)
		goto unlock;

	if (report.dsrvovflint)
		dev_err(core->dev, "data services system overflow\n");
	if (report.buff_count == 0) /* no buffer available */
		goto unlock;

	report.payload = kzalloc(SI468X_SERVICE_DATA_MAX_LENGTH, GFP_KERNEL);
	if (!report.payload)
		goto unlock;

	err = si468x_core_cmd_dab_get_digital_service_data(core, false, true, &report);
	if (err < 0)
		goto free_kmem;

	if ((report.data_src == 2) && ((report.payload[0] & 0x7f) == 0))
		strscpy(core->si468x_dls_message,
			report.payload + 2,
			SI468X_DAB_DL_PLUS_MAX_TEXT_LENGTH);
free_kmem:
	kfree(report.payload);
unlock:
	si468x_core_unlock(core);
	mutex_lock(&core->digital_service_drainer_status_lock);
	core->digital_service_drainer_is_working = false;
	mutex_unlock(&core->digital_service_drainer_status_lock);
}

static int si468x_cmd_clear_stc(struct si468x_core *core)
{
	int err;
	struct si468x_rsq_status_args args = {
		.rsqack		= false,
		.digradack	= false,
		.attune		= false,
		.cancel		= false,
		.fiberrack	= false,
		.stcack		= true,
	};

	switch (core->power_up_parameters.func) {
	case SI468X_FUNC_AM_RECEIVER:
		err = si468x_core_cmd_am_rsq_status(core, &args, NULL);
		break;
	case SI468X_FUNC_FM_RECEIVER:
		err = si468x_core_cmd_fm_rsq_status(core, &args, NULL);
		break;
	case SI468X_FUNC_DAB_RECEIVER:
		err = si468x_core_cmd_dab_rsq_status(core, &args, NULL);
		break;
	default:
		err = -EINVAL;
	}

	return err;
}

static int si468x_cmd_tune_seek_freq(struct si468x_core *core,
				     uint8_t cmd,
				     const uint8_t args[], size_t argn,
				     uint8_t *resp, size_t respn)
{
	int err;

	atomic_set(&core->stc, 0);
	err = si468x_core_send_command(core, cmd, args, argn, resp, respn,
				       SI468X_TIMEOUT_TUNE);
	if (!(err < 0)) {
		wait_event_killable(core->tuning,
				    atomic_read(&core->stc));
		si468x_cmd_clear_stc(core);
	}

	return err;
}

/**
 * si468x_cmd_set_property() - send 'SET_PROPERTY' command to the device
 * @core:    device to send the command to
 * @property: property address
 * @value:    property value
 *
 * Function returns 0 on succsess and negative error code on
 * failure
 */
int si468x_core_cmd_set_property(struct si468x_core *core,
				 u16 property, u16 value)
{
	u8       resp[CMD_SET_PROPERTY_NRESP];
	const u8 args[CMD_SET_PROPERTY_NARGS] = {
		0x00,
		lsb(property),
		msb(property),
		lsb(value),
		msb(value),
	};

	if (core->chip_state != SI468X_STATE_APPLICATION_RUNNING) {
		dev_err(core->dev, "Can not set_property - Application is not running\n");
		return -EIO;
	}

	return si468x_core_send_command(core, CMD_SET_PROPERTY,
					args, ARRAY_SIZE(args),
					resp, ARRAY_SIZE(resp),
					SI468X_DEFAULT_TIMEOUT);
}
EXPORT_SYMBOL_GPL(si468x_core_cmd_set_property);

/**
 * si468x_cmd_get_property() - send 'GET_PROPERTY' command to the device
 * @core:    device to send the command to
 * @property: property address
 *
 * Function return the value of property as u16 on success or a
 * negative error on failure
 */
int si468x_core_cmd_get_property(struct si468x_core *core, u16 property)
{
	int err;
	u8       resp[CMD_GET_PROPERTY_NRESP];
	const u8 args[CMD_GET_PROPERTY_NARGS] = {
		0x01,
		lsb(property),
		msb(property),
	};

	if (core->chip_state != SI468X_STATE_APPLICATION_RUNNING) {
		dev_err(core->dev, "Can not get_property - Application is not running\n");
		return -EIO;
	}

	err = si468x_core_send_command(core, CMD_GET_PROPERTY,
				       args, ARRAY_SIZE(args),
				       resp, ARRAY_SIZE(resp),
				       SI468X_DEFAULT_TIMEOUT);
	if (err < 0)
		return err;
	else
		return get_unaligned_le16(resp + 4);
}
EXPORT_SYMBOL_GPL(si468x_core_cmd_get_property);


int si468x_core_flash_nvm(struct si468x_core *core,
			  const char *name)
{
	int err;

	if (core->nvm_state == SI468X_STATE_NVM_UNINITIALIZED) {
		dev_err(core->dev, "SI468X_STATE_NVM_UNINITIALIZED\n");
		return -EIO;
	}
	err = si468x_cmd_load_firmware(core,
				       name,
				       SI468X_LOAD_TO_FLASH);
	if (err < 0)
		dev_err(core->dev,
			"Failed to flash %s (err = %d)\n",
			name, err);
	return err;
}

static ssize_t si468x_nvram_store(struct device *dev,
				   struct device_attribute *attr,
				   const char *buf, size_t count)
{
	u8       resp[CMD_FLASH_LOAD_NRESP];
	const u8 args[CMD_FLASH_LOAD_NARGS] = {
		0xff,
		0xde,
		0xc0,
	};
	int err;
	struct si468x_core *core = dev_get_drvdata(dev);

	si468x_core_lock(core);
	core->power_up_parameters.func = SI468X_FUNC_BOOTLOADER;
	err = si473x_core_set_power_state(core, SI468X_STATE_POWER_UP);
	if (err < 0)
		goto exit;

	err = si468x_cmd_set_nvm_parameters(core);
	if (err < 0)
		goto powerdown;

	if (sysfs_streq(buf, "erase")) {
		err = si468x_core_send_command(core, CMD_FLASH_LOAD,
			       args, ARRAY_SIZE(args),
			       resp, ARRAY_SIZE(resp),
			       SI468X_DEFAULT_TIMEOUT);
		if (err < 0)
			dev_err(core->dev,
				"Failed to erase nvram (err = %d)\n", err);
	} else if (sysfs_streq(buf, si468x_func_string_table[SI468X_FUNC_MINI_BOOT])) {
		err = si468x_core_flash_nvm(core, si468x_func_string_table[SI468X_FUNC_MINI_BOOT]);
	} else if (sysfs_streq(buf, si468x_func_string_table[SI468X_FUNC_BOOTLOADER])) {
		err = si468x_core_flash_nvm(core, si468x_func_string_table[SI468X_FUNC_BOOTLOADER]);
	} else if (sysfs_streq(buf, si468x_func_string_table[SI468X_FUNC_AM_RECEIVER])) {
		err = si468x_core_flash_nvm(core, si468x_func_string_table[SI468X_FUNC_AM_RECEIVER]);
	} else if (sysfs_streq(buf, si468x_func_string_table[SI468X_FUNC_FM_RECEIVER])) {
		err = si468x_core_flash_nvm(core, si468x_func_string_table[SI468X_FUNC_FM_RECEIVER]);
	} else if (sysfs_streq(buf, si468x_func_string_table[SI468X_FUNC_DAB_RECEIVER])) {
		err = si468x_core_flash_nvm(core, si468x_func_string_table[SI468X_FUNC_DAB_RECEIVER]);
	} else {
		dev_err(core->dev, "si468x_nvram invalid\n");
		err = -EINVAL;
	}
powerdown:
	si473x_core_set_power_state(core, SI468X_STATE_POWER_DOWN);
exit:
	si468x_core_unlock(core);

	return (err < 0) ? err : count;
}

static ssize_t si468x_property_store(struct device *dev,
				   struct device_attribute *attr,
				   const char *buf, size_t count)
{
	int num, err;
	u16 reg, val;
	struct si468x_core *core = dev_get_drvdata(dev);

	num = sscanf(buf, "0x%hX 0x%hX",&reg, &val);

	switch (num) {
	case 2:
		si468x_core_lock(core);
		err = si468x_core_cmd_set_property(core, reg, val);
		si468x_core_unlock(core);
		dev_info(core->dev, "si468x_property_store reg%X val%X err%X\n", reg, val, err);
		return (err < 0) ? err : count;
	default:
		dev_err(core->dev, "si468x_property_store invalid\n");
		return -EINVAL;
	}
}

static ssize_t si468x_service_list_show(struct device *dev,
					struct device_attribute *attr,
					char *buf)
{
	struct si468x_dab_channel *ptr;
	sprintf(buf, "Service List:\n");
	sprintf(buf + strlen(buf), "    low "
				   "      high "
				   "spacing "
				   "(for use with v4l2-ctl --freq-seek)\n");
	sprintf(buf + strlen(buf), "    MHz "
				   "Service ID "
				   "SubChId "
				   "FIC "
				   "Strength "
				   "Country "
				   "Version "
				   "started "
				   "Label            \n");
	list_for_each_entry(ptr, &si468x_dab_channel_list, list) {
		sprintf(buf + strlen(buf), "%3d.%03d %10d %7d %3d %8d %7d %7d %s %s\n",
			ptr->frequency / 1000,
			ptr->frequency % 1000,
			ptr->service_id,
			ptr->component_info.sub_ch_id,
			ptr->fic_quality,
			ptr->signal_strength,
			ptr->country_id,
			ptr->version,
			(ptr->is_started) ? "   *   " : "   -   ",
			ptr->service_label);
	}
	return strlen(buf);

}static ssize_t si468x_dynamic_label_show(struct device *dev,
					struct device_attribute *attr,
					char *buf)
{
	struct si468x_core *core = dev_get_drvdata(dev);

	si468x_core_lock(core);
	snprintf(buf, SI468X_DAB_DL_PLUS_MAX_TEXT_LENGTH,
		 "%s\n", core->si468x_dls_message);
	si468x_core_unlock(core);
	return strlen(buf);
}

static DEVICE_ATTR_WO(si468x_nvram);
static DEVICE_ATTR_WO(si468x_property);
static DEVICE_ATTR_RO(si468x_service_list);
static DEVICE_ATTR_RO(si468x_dynamic_label);

static struct attribute *si468x_attributes[] = {
	&dev_attr_si468x_nvram.attr,
	&dev_attr_si468x_property.attr,
	&dev_attr_si468x_service_list.attr,
	&dev_attr_si468x_dynamic_label.attr,
	NULL,
};

static const struct attribute_group si468x_attr_group = {
	.attrs = si468x_attributes,
};

/**
 * si468x_cmd_am_rsq_status - send 'AM_RSQ_STATUS' command to the
 * device
 * @core  - device to send the command to
 * @rsqack - if set command clears RSQINT, SNRINT, SNRLINT, RSSIHINT,
 *           RSSSILINT, BLENDINT, MULTHINT and MULTLINT
 * @attune - when set the values in the status report are the values
 *           that were calculated at tune
 * @cancel - abort ongoing seek/tune opertation
 * @stcack - clear the STCINT bin in status register
 * @report - all signal quality information retured by the command
 *           (if NULL then the output of the command is ignored)
 *
 * Function returns 0 on success and negative error code on failure
 */
int si468x_core_cmd_am_rsq_status(struct si468x_core *core,
				  struct si468x_rsq_status_args *rsqargs,
				  struct si468x_rsq_status_report *report)
{
	int err;
	u8       resp[CMD_AM_RSQ_STATUS_NRESP];
	const u8 args[CMD_AM_RSQ_STATUS_NARGS] = {
		rsqargs->rsqack << 3 | rsqargs->attune << 2 |
		rsqargs->cancel << 1 | rsqargs->stcack,
	};

	err = si468x_core_send_command(core, CMD_AM_RSQ_STATUS,
				       args, ARRAY_SIZE(args),
				       resp, ARRAY_SIZE(resp),
				       SI468X_DEFAULT_TIMEOUT);
	/*
	 * Besides getting received signal quality information this
	 * command can be used to just acknowledge different interrupt
	 * flags in those cases it is useless to copy and parse
	 * received data so user can pass NULL, and thus avoid
	 * unnecessary copying.
	 */
	if (!report)
		return err;

	report->hdlevelhint	= 0x20 & resp[4];
	report->hdlevellint	= 0x10 & resp[4];
	report->snrhint		= 0x08 & resp[4];
	report->snrlint		= 0x04 & resp[4];
	report->rssihint	= 0x02 & resp[4];
	report->rssilint	= 0x01 & resp[4];

	report->bltf		= 0x80 & resp[5];
	report->hddetected	= 0x20 & resp[5];
	report->flt_hddetected	= 0x08 & resp[5];
	report->afcrl		= 0x02 & resp[5];
	report->valid		= 0x01 & resp[5];

	report->readfreq	= get_unaligned_le16(resp + 6);
	report->freqoff		= resp[8];
	report->rssi		= resp[9];
	report->snr		= resp[10];
	report->mod		= resp[11];
	report->readantcap	= get_unaligned_le16(resp + 12);
	report->hdlevel		= resp[15];
	report->flt_hdlevel	= resp[16];

	return err;
}
EXPORT_SYMBOL_GPL(si468x_core_cmd_am_rsq_status);

int si468x_core_cmd_fm_rsq_status(struct si468x_core *core,
					struct si468x_rsq_status_args *rsqargs,
					struct si468x_rsq_status_report *report)
{
	int err;
	u8       resp[CMD_FM_RSQ_STATUS_NRESP];
	const u8 args[CMD_FM_RSQ_STATUS_NARGS] = {
		rsqargs->rsqack << 3 | rsqargs->attune << 2 |
		rsqargs->cancel << 1 | rsqargs->stcack,
	};

	err = si468x_core_send_command(core, CMD_FM_RSQ_STATUS,
				       args, ARRAY_SIZE(args),
				       resp, ARRAY_SIZE(resp),
				       SI468X_DEFAULT_TIMEOUT);
	/*
	 * Besides getting received signal quality information this
	 * command can be used to just acknowledge different interrupt
	 * flags in those cases it is useless to copy and parse
	 * received data so user can pass NULL, and thus avoid
	 * unnecessary copying.
	 */
	if (err < 0 || report == NULL)
		return err;

	report->hdlevelhint	= 0x20 & resp[4];
	report->hdlevellint	= 0x10 & resp[4];
	report->snrhint		= 0x08 & resp[4];
	report->snrlint		= 0x04 & resp[4];
	report->rssihint	= 0x02 & resp[4];
	report->rssilint	= 0x01 & resp[4];

	report->bltf		= 0x80 & resp[5];
	report->hddetected	= 0x20 & resp[5];
	report->flt_hddetected	= 0x08 & resp[5];
	report->afcrl		= 0x02 & resp[5];
	report->valid		= 0x01 & resp[5];

	report->readfreq	= get_unaligned_le16(resp + 6);
	report->freqoff		= resp[8];
	report->rssi		= resp[9];
	report->snr		= resp[10];
	report->mult		= resp[11];
	report->readantcap	= get_unaligned_le16(resp + 12);
	report->hdlevel		= resp[15];
	report->flt_hdlevel	= resp[16];

	return err;
}
EXPORT_SYMBOL_GPL(si468x_core_cmd_fm_rsq_status);

int si468x_core_cmd_dab_rsq_status(struct si468x_core *core,
					struct si468x_rsq_status_args *rsqargs,
					struct si468x_rsq_status_report *report)
{
	int err;
	u8       resp[CMD_DAB_DIGRAD_STATUS_NRESP];
	const u8 args[CMD_DAB_DIGRAD_STATUS_NARGS] = {
		rsqargs->digradack << 3 | rsqargs->attune << 2 |
		rsqargs->fiberrack << 1 | rsqargs->stcack,
	};
	err = si468x_core_send_command(core, CMD_DAB_DIGRAD_STATUS,
				       args, ARRAY_SIZE(args),
				       resp, ARRAY_SIZE(resp),
				       SI468X_DEFAULT_TIMEOUT);
	/*
	 * Besides getting received signal quality information this
	 * command can be used to just acknowledge different interrupt
	 * flags in those cases it is useless to copy and parse
	 * received data so user can pass NULL, and thus avoid
	 * unnecessary copying.
	 */
	if (err < 0 || report == NULL)
		return err;

	report->ficerrint	= 0x08 & resp[4];
	report->acqint		= 0x04 & resp[4];
	report->rssihint	= 0x02 & resp[4];
	report->rssilint	= 0x01 & resp[4];

	report->ficerr		= 0x08 & resp[5];
	report->acq		= 0x04 & resp[5];
	report->valid		= 0x01 & resp[5];

	report->rssi		= resp[6];
	report->snr		= resp[7];
	report->fic_quality	= resp[8];
	report->cnr		= resp[9];
	report->fib_error_count	= get_unaligned_le16(resp + 10);
	report->readfreq	= get_unaligned_le32(resp + 12);
	report->tune_index	= resp[16];
	report->fft_offset	= resp[17];
	report->readantcap	= get_unaligned_le16(resp + 18);
	report->cu_level	= get_unaligned_le16(resp + 20);
	report->fast_dect	= resp[22];

	return err;
}
EXPORT_SYMBOL_GPL(si468x_core_cmd_dab_rsq_status);

int si468x_core_cmd_am_acf_status(struct si468x_core *core,
				  struct si468x_acf_status_report *report)
{
	int err;
	u8       resp[CMD_AM_ACF_STATUS_NRESP];
	const u8 args[CMD_AM_ACF_STATUS_NARGS] = {
		SI468X_ACF_ACK_INT,
	};

	if (!report)
		return -EINVAL;

	err = si468x_core_send_command(core, CMD_AM_ACF_STATUS,
				       args, ARRAY_SIZE(args),
				       resp, ARRAY_SIZE(resp),
				       SI468X_DEFAULT_TIMEOUT);
	if (err < 0)
		return err;

	report->hicut_int	= resp[4] & SI468X_ACF_HICUT_INT;
	report->softmute_int	= resp[4] & SI468X_ACF_SOFTMUTE_INT;
	report->hicut_conv	= resp[5] & SI468X_ACF_HICUT_CONV;
	report->softmute_conv	= resp[5] & SI468X_ACF_SOFTMUTE_CONV;
	report->hicut_state	= resp[5] & SI468X_ACF_HICUT_STATE;
	report->softmute_state	= resp[5] & SI468X_ACF_SOFTMUTE_STATE;
	report->smattn		= resp[6] & SI468X_ACF_SMATTN;
	report->hicut		= resp[7];
	report->lowcut		= resp[8];

	return err;
}
EXPORT_SYMBOL_GPL(si468x_core_cmd_am_acf_status);

int si468x_core_cmd_fm_acf_status(struct si468x_core *core,
				  struct si468x_acf_status_report *report)
{
	int err;
	u8       resp[CMD_FM_ACF_STATUS_NRESP];
	const u8 args[CMD_FM_ACF_STATUS_NARGS] = {
		SI468X_ACF_ACK_INT,
	};

	if (!report)
		return -EINVAL;

	err = si468x_core_send_command(core, CMD_FM_ACF_STATUS,
				       args, ARRAY_SIZE(args),
				       resp, ARRAY_SIZE(resp),
				       SI468X_DEFAULT_TIMEOUT);
	if (err < 0)
		return err;

	report->blend_int	= resp[4] & SI468X_ACF_BLEND_INT;
	report->hicut_int	= resp[4] & SI468X_ACF_HICUT_INT;
	report->softmute_int	= resp[4] & SI468X_ACF_SOFTMUTE_INT;
	report->blend_conv	= resp[5] & SI468X_ACF_BLEND_CONV;
	report->hicut_conv	= resp[5] & SI468X_ACF_HICUT_CONV;
	report->softmute_conv	= resp[5] & SI468X_ACF_SOFTMUTE_CONV;
	report->blend_state	= resp[5] & SI468X_ACF_BLEND_STATE;
	report->hicut_state	= resp[5] & SI468X_ACF_HICUT_STATE;
	report->softmute_state	= resp[5] & SI468X_ACF_SOFTMUTE_STATE;
	report->smattn		= resp[6] & SI468X_ACF_SMATTN;
	report->hicut		= resp[7];
	report->pilot		= resp[8] & SI468X_ACF_PILOT;
	report->stblend		= resp[8] & SI468X_ACF_STBLEND;

	return err;
}
EXPORT_SYMBOL_GPL(si468x_core_cmd_fm_acf_status);

int si468x_core_cmd_dab_acf_status(struct si468x_core *core,
				  struct si468x_acf_status_report *report)
{
	int err;
	u8       resp[CMD_DAB_ACF_STATUS_NRESP];
	const u8 args[CMD_DAB_ACF_STATUS_NARGS] = {
			0,
	};

	if (!report)
		return -EINVAL;

	err = si468x_core_send_command(core, CMD_DAB_ACF_STATUS,
				       args, ARRAY_SIZE(args),
				       resp, ARRAY_SIZE(resp),
				       SI468X_DEFAULT_TIMEOUT);
	if (err < 0)
		return err;

	report->rfu1		= resp[4];
	report->rfu2		= resp[5];
	report->audio_level	= get_unaligned_le16(resp + 6);
	report->cmft_noise_level= get_unaligned_le16(resp + 8);

	return err;
}
EXPORT_SYMBOL_GPL(si468x_core_cmd_dab_acf_status);

int si468x_core_cmd_dab_event_status(struct si468x_core *core,
				     struct si468x_event_status_args *eventargs,
				     struct si468x_event_status_report *report)
{
	int err;
	u8       resp[CMD_DAB_GET_EVENT_STATUS_NRESP];
	const u8 args[CMD_DAB_GET_EVENT_STATUS_NARGS] = {
			eventargs->eventack << 0,
	};

	err = si468x_core_send_command(core, CMD_DAB_GET_EVENT_STATUS,
				       args, ARRAY_SIZE(args),
				       resp, ARRAY_SIZE(resp),
				       SI468X_DEFAULT_TIMEOUT);
	if (err < 0 || report == NULL)
		return err;

	report->recfgint	= resp[4] & SI468X_EVENT_RECFGINT;
	report->recfgwrnint	= resp[4] & SI468X_EVENT_RECFGWRNINT;
	report->annoint		= resp[4] & SI468X_EVENT_ANNOINT;
	report->oeservint	= resp[4] & SI468X_EVENT_OESERVINT;
	report->servlinkint	= resp[4] & SI468X_EVENT_SERVLINKINT;
	report->freqinfoint	= resp[4] & SI468X_EVENT_FREQINFOINT;
	report->svrlistint	= resp[4] & SI468X_EVENT_SVRLISTINT;
	report->anno		= resp[5] & SI468X_EVENT_ANNO;
	report->oeserv		= resp[5] & SI468X_EVENT_OESERV;
	report->servlink	= resp[5] & SI468X_EVENT_SERVLINK;
	report->freq_info	= resp[5] & SI468X_EVENT_FREQ_INFO;
	report->svrlist		= resp[5] & SI468X_EVENT_SVRLIST;
	report->svrlistver	= get_unaligned_le16(resp + 6);

	return err;
}
EXPORT_SYMBOL_GPL(si468x_core_cmd_dab_event_status);

int si468x_core_cmd_dab_get_service_list(struct si468x_core *core,
					 struct si468x_dab_service_list *list)
{
	int err;
	int srvnr, compnr, respptr;
	u8       resp[CMD_GET_DIGITAL_SERVICE_LIST_NRESP];
	u8       *fullresp;
	const u8 args[CMD_GET_DIGITAL_SERVICE_LIST_NARGS] = {
			0,
	};

	err = si468x_core_send_command(core, CMD_GET_DIGITAL_SERVICE_LIST,
				       args, ARRAY_SIZE(args),
				       resp, ARRAY_SIZE(resp),
				       SI468X_DEFAULT_TIMEOUT);
	if (err < 0 || list == NULL)
		return err;

	fullresp = kmalloc(get_unaligned_le16(resp + 4),
			   GFP_KERNEL);
	if (!fullresp)
		return -ENOMEM;
	err = si468x_core_send_command(core, CMD_GET_DIGITAL_SERVICE_LIST,
				       args, ARRAY_SIZE(args),
				       fullresp,
				       CMD_GET_DIGITAL_SERVICE_LIST_NRESP +
				       get_unaligned_le16(resp + 4) - 2,
				       SI468X_DEFAULT_TIMEOUT);
	if (err < 0)
		goto free_kmem;

	list->version = get_unaligned_le16(fullresp + 6);
	list->number_of_services = fullresp[8] & 0x1f; /* max: 32 */
	respptr = CMD_GET_DIGITAL_SERVICE_LIST_NRESP + 6;
	for (srvnr = 0;
	     srvnr < list->number_of_services;
	     srvnr++) {
		if (fullresp[respptr + 4] & 0x01) {
			list->si468x_dab_service_info[srvnr].is_data_service = true;
			list->si468x_dab_service_info[srvnr].is_audio_service = false;
		} else {
			list->si468x_dab_service_info[srvnr].is_data_service = false;
			list->si468x_dab_service_info[srvnr].is_audio_service = true;
		}
		if (list->si468x_dab_service_info[srvnr].is_audio_service) {
			list->si468x_dab_service_info[srvnr].service_id = 
			get_unaligned_le32(fullresp + respptr) & 0xfff;
			list->si468x_dab_service_info[srvnr].country_id = 
			(get_unaligned_le32(fullresp + respptr) >> 12) & 0xf;
		} else {
			list->si468x_dab_service_info[srvnr].service_id = 
			get_unaligned_le32(fullresp + respptr) & 0xfffff;
			list->si468x_dab_service_info[srvnr].country_id = 
			(get_unaligned_le32(fullresp + respptr) >> 20) & 0xf;
			list->si468x_dab_service_info[srvnr].extended_country_code = 
			(get_unaligned_le32(fullresp + respptr) >> 24) & 0xff;
		}

		list->si468x_dab_service_info[srvnr].srv_linking_info_flag = 
		(fullresp[respptr + 4] >> 6) & 0x01;
		list->si468x_dab_service_info[srvnr].program_type = 
		(fullresp[respptr + 4] >> 1) & 0x1f;
		list->si468x_dab_service_info[srvnr].is_local_service = 
		(fullresp[respptr + 5] >> 7) & 0x01;
		list->si468x_dab_service_info[srvnr].control_access_id = 
		(fullresp[respptr + 5] >> 4) & 0x07;
		list->si468x_dab_service_info[srvnr].number_of_components = 
		(fullresp[respptr + 5] >> 0) & 0x0f;
		list->si468x_dab_service_info[srvnr].si_charset = 
		fullresp[respptr + 6] & 0x0f;
		strscpy(list->si468x_dab_service_info[srvnr].service_label,
			&fullresp[respptr + 8],
			sizeof(list->si468x_dab_service_info[srvnr].service_label));
		respptr += 24;
		for (compnr = 0;
		     compnr < list->si468x_dab_service_info[srvnr].number_of_components;
		     compnr++) {
			list->si468x_dab_service_info[srvnr].si468x_dab_component_info[compnr].tm_id =
			(get_unaligned_le16(fullresp + respptr) >> 14) & 0x03;
			switch (list->si468x_dab_service_info[srvnr].si468x_dab_component_info[compnr].tm_id) {
			case 0:
			case 1:
				list->si468x_dab_service_info[srvnr].si468x_dab_component_info[compnr].sub_ch_id =
				get_unaligned_le16(fullresp + respptr) & 0x3f;
				break;
			case 2:
				list->si468x_dab_service_info[srvnr].si468x_dab_component_info[compnr].fidc_id =
				get_unaligned_le16(fullresp + respptr) & 0x3f;
				break;
			case 3:
				list->si468x_dab_service_info[srvnr].si468x_dab_component_info[compnr].dg_flag =
				(get_unaligned_le16(fullresp + respptr) >> 13) & 0x01;
				list->si468x_dab_service_info[srvnr].si468x_dab_component_info[compnr].sc_id =
				get_unaligned_le16(fullresp+ respptr) & 0xfff;
				break;
			default:
				break;
			}
			if (list->si468x_dab_service_info[srvnr].is_audio_service) {
				list->si468x_dab_service_info[srvnr].si468x_dab_component_info[compnr].audio_service_type =
				(fullresp[respptr + 2] >> 2) & 0x3f;
			} else {
				list->si468x_dab_service_info[srvnr].si468x_dab_component_info[compnr].data_service_type =
				(fullresp[respptr + 2] >> 2) & 0x3f;
			}
			if (fullresp[respptr + 2] & 0x2) {
				list->si468x_dab_service_info[srvnr].si468x_dab_component_info[compnr].is_primary = false;
				list->si468x_dab_service_info[srvnr].si468x_dab_component_info[compnr].is_secondary = true;
			} else {
				list->si468x_dab_service_info[srvnr].si468x_dab_component_info[compnr].is_primary = true;
				list->si468x_dab_service_info[srvnr].si468x_dab_component_info[compnr].is_secondary = false;
			}
			list->si468x_dab_service_info[srvnr].si468x_dab_component_info[compnr].access_control_flag =
			fullresp[respptr + 2] & 0x01;
			list->si468x_dab_service_info[srvnr].si468x_dab_component_info[compnr].mua_info_valid =
			fullresp[respptr + 3] & 0x01;
			respptr += 4;
		}
	}

free_kmem:
	kfree(fullresp);
	return err;
}
EXPORT_SYMBOL_GPL(si468x_core_cmd_dab_get_service_list);

/**
 * si468x_cmd_fm_rds_status - send 'FM_RDS_STATUS' command to the
 * device
 * @core - device to send the command to
 * @status_only - if set the data is not removed from RDSFIFO,
 *                RDSFIFOUSED is not decremented and data in all the
 *                rest RDS data contains the last valid info received
 * @mtfifo if set the command clears RDS receive FIFO
 * @intack if set the command clards the RDSINT bit.
 *
 * Function returns 0 on success and negative error code on failure
 */
int si468x_core_cmd_fm_rds_status(struct si468x_core *core,
				  bool status_only,
				  bool mtfifo,
				  bool intack,
				  struct si468x_rds_status_report *report)
{
	int err;
	u8       resp[CMD_FM_RDS_STATUS_NRESP];
	const u8 args[CMD_FM_RDS_STATUS_NARGS] = {
		status_only << 2 | mtfifo << 1 | intack,
	};

	err = si468x_core_send_command(core, CMD_FM_RDS_STATUS,
				       args, ARRAY_SIZE(args),
				       resp, ARRAY_SIZE(resp),
				       SI468X_DEFAULT_TIMEOUT);
	/*
	 * Besides getting RDS status information this command can be
	 * used to just acknowledge different interrupt flags in those
	 * cases it is useless to copy and parse received data so user
	 * can pass NULL, and thus avoid unnecessary copying.
	 */
	if (err < 0 || report == NULL)
		return err;

	report->rdstpptyint	= 0x10 & resp[4];
	report->rdspiint	= 0x08 & resp[4];
	report->rdssyncint	= 0x02 & resp[4];
	report->rdsfifoint	= 0x01 & resp[4];

	report->tpptyvalid	= 0x10 & resp[5];
	report->pivalid		= 0x08 & resp[5];
	report->rdssync		= 0x02 & resp[5];
	report->rdsfifolost	= 0x01 & resp[5];

	report->tp		= 0x20 & resp[6];
	report->pty		= 0x1f & resp[6];

	report->pi		= get_unaligned_le16(resp + 8);
	report->rdsfifoused	= resp[10];

	report->ble[V4L2_RDS_BLOCK_A]	= 0xc0 & resp[11];
	report->ble[V4L2_RDS_BLOCK_B]	= 0x30 & resp[11];
	report->ble[V4L2_RDS_BLOCK_C]	= 0x0c & resp[11];
	report->ble[V4L2_RDS_BLOCK_D]	= 0x03 & resp[11];

	report->rds[V4L2_RDS_BLOCK_A].block = V4L2_RDS_BLOCK_A;
	report->rds[V4L2_RDS_BLOCK_A].lsb = resp[12];
	report->rds[V4L2_RDS_BLOCK_A].msb = resp[13];

	report->rds[V4L2_RDS_BLOCK_B].block = V4L2_RDS_BLOCK_B;
	report->rds[V4L2_RDS_BLOCK_B].lsb = resp[14];
	report->rds[V4L2_RDS_BLOCK_B].msb = resp[15];

	report->rds[V4L2_RDS_BLOCK_C].block = V4L2_RDS_BLOCK_C;
	report->rds[V4L2_RDS_BLOCK_C].lsb = resp[16];
	report->rds[V4L2_RDS_BLOCK_C].msb = resp[17];

	report->rds[V4L2_RDS_BLOCK_D].block = V4L2_RDS_BLOCK_D;
	report->rds[V4L2_RDS_BLOCK_D].lsb = resp[18];
	report->rds[V4L2_RDS_BLOCK_D].msb = resp[19];

	return err;
}
EXPORT_SYMBOL_GPL(si468x_core_cmd_fm_rds_status);

int si468x_core_cmd_fm_rds_blockcount(struct si468x_core *core,
				bool clear,
				struct si468x_rds_blockcount_report *report)
{
	int err;
	u8       resp[CMD_FM_RDS_BLOCKCOUNT_NRESP];
	const u8 args[CMD_FM_RDS_BLOCKCOUNT_NARGS] = {
		clear,
	};

	if (!report)
		return -EINVAL;

	err = si468x_core_send_command(core, CMD_FM_RDS_BLOCKCOUNT,
				       args, ARRAY_SIZE(args),
				       resp, ARRAY_SIZE(resp),
				       SI468X_DEFAULT_TIMEOUT);

	if (!(err < 0)) {
		report->expected	= get_unaligned_le16(resp + 4);
		report->received	= get_unaligned_le16(resp + 6);
		report->uncorrectable	= get_unaligned_le16(resp + 8);
	}

	return err;
}
EXPORT_SYMBOL_GPL(si468x_core_cmd_fm_rds_blockcount);

/**
 * si468x_cmd_am_seek_start - send 'FM_SEEK_START' command to the
 * device
 * @core  - device to send the command to
 * @seek - seekup, wrap and other parameters
 *
 * This function begins search for a valid station. The station is
 * considered valid when 'FM_VALID_SNR_THRESHOLD' and
 * 'FM_VALID_RSSI_THRESHOLD' and 'FM_VALID_MAX_TUNE_ERROR' criteria
 * are met.
 *
 * Function returns 0 on success and negative error code on failure
 */
int si468x_core_cmd_am_seek_start(struct si468x_core *core,
				  const struct v4l2_hw_freq_seek *seek,
				  struct si468x_tune_freq_args *tuneargs)
{
	u8       resp[CMD_AM_SEEK_START_NRESP];
	const u8 args[CMD_AM_SEEK_START_NARGS] = {
		(tuneargs->tunemode << 2) | (tuneargs->injside),
		(seek->seek_upward & 0x01) << 1 |
		(seek->wrap_around & 0x01) << 0,
		0,
		lsb(tuneargs->antcap),
		msb(tuneargs->antcap),
	};

	return si468x_cmd_tune_seek_freq(core,  CMD_AM_SEEK_START,
					 args, sizeof(args),
					 resp, sizeof(resp));
}
EXPORT_SYMBOL_GPL(si468x_core_cmd_am_seek_start);

/**
 * si468x_cmd_fm_seek_start - send 'FM_SEEK_START' command to the
 * device
 * @core  - device to send the command to
 * @seek - seekup, wrap and other parameters
 *
 * This function begins search for a valid station. The station is
 * considered valid when 'FM_VALID_SNR_THRESHOLD' and
 * 'FM_VALID_RSSI_THRESHOLD' and 'FM_VALID_MAX_TUNE_ERROR' criteria
 * are met.
 *
 * Function returns 0 on success and negative error code on failure
 */
int si468x_core_cmd_fm_seek_start(struct si468x_core *core,
				  const struct v4l2_hw_freq_seek *seek,
				  struct si468x_tune_freq_args *tuneargs)
{
	u8       resp[CMD_FM_SEEK_START_NRESP];
	const u8 args[CMD_FM_SEEK_START_NARGS] = {
		(tuneargs->tunemode << 2) | (tuneargs->injside),
		(seek->seek_upward & 0x01) << 1 |
		(seek->wrap_around & 0x01) << 0,
		0,
		lsb(tuneargs->antcap),
		msb(tuneargs->antcap),
	};
	return si468x_cmd_tune_seek_freq(core, CMD_FM_SEEK_START,
					 args, sizeof(args),
					 resp, sizeof(resp));
}
EXPORT_SYMBOL_GPL(si468x_core_cmd_fm_seek_start);

/**
 * si468x_cmd_dab_seek_start - tune to next station in the list
 * @core  - device to send the command to
 * @seek - seekup, wrap and other parameters
 *
 * Function returns 0 on success and negative error code on failure
 */
int si468x_core_cmd_dab_seek_start(struct si468x_core *core,
				   const struct v4l2_hw_freq_seek *seek,
				   struct si468x_tune_freq_args *tuneargs)
{
	int err;
	u32 frequency = seek->rangelow / (FREQ_MUL / 1000); /* kHz */
	u32 service_id = seek->rangehigh / FREQ_MUL;
	u8  sub_ch_id = seek->spacing;
	struct si468x_dab_channel *ptr;

	if (list_empty(&si468x_dab_channel_list))
		return -EINVAL;
	/* stop service first */
	list_for_each_entry(ptr, &si468x_dab_channel_list, list) {
		if(ptr->is_started) {
			err = si468x_core_cmd_dab_stop_service(core,
							       ptr);
			if (err < 0)
				return err;
			break;
		}
	}

	if (frequency == 0) {
		if (seek->seek_upward) {
			if (!list_is_last(&ptr->list, &si468x_dab_channel_list))
				ptr = list_next_entry(ptr, list);
			else
				if (seek->wrap_around)
					ptr = list_first_entry(&si468x_dab_channel_list,
							struct si468x_dab_channel, list);
		} else {
			if (!list_is_first(&ptr->list, &si468x_dab_channel_list))
				ptr = list_prev_entry(ptr, list);
			else
				if (seek->wrap_around)
					ptr = list_last_entry(&si468x_dab_channel_list,
							struct si468x_dab_channel, list);
		}
		return si468x_core_cmd_dab_start_service(core, ptr);
	}

	/* find matching entry */
	list_for_each_entry(ptr, &si468x_dab_channel_list, list) {
		if (ptr->frequency == frequency &&
		    ptr->service_id == service_id &&
		    ptr->component_info.sub_ch_id == sub_ch_id) {
			err = si468x_core_cmd_dab_start_service(core, ptr);
			break;
		}
	}

	return err;
}
EXPORT_SYMBOL_GPL(si468x_core_cmd_dab_seek_start);

int si468x_core_cmd_am_tune_freq(struct si468x_core *core,
					struct si468x_tune_freq_args *tuneargs)
{
	u8       resp[CMD_AM_TUNE_FREQ_NRESP];
	const u8 args[CMD_AM_TUNE_FREQ_NARGS] = {
		(tuneargs->tunemode << 2) | (tuneargs->injside),
		lsb(tuneargs->freq),
		msb(tuneargs->freq),
		lsb(tuneargs->antcap),
		msb(tuneargs->antcap),
	};

	return si468x_cmd_tune_seek_freq(core, CMD_AM_TUNE_FREQ,
					 args, sizeof(args),
					 resp, sizeof(resp));
}
EXPORT_SYMBOL_GPL(si468x_core_cmd_am_tune_freq);

int si468x_core_cmd_fm_tune_freq(struct si468x_core *core,
					struct si468x_tune_freq_args *tuneargs)
{
	u8       resp[CMD_FM_TUNE_FREQ_NRESP];
	const u8 args[CMD_FM_TUNE_FREQ_NARGS] = {
		(tuneargs->direct_tune << 5) | (tuneargs->tunemode << 2) |
		(tuneargs->injside),
		lsb(tuneargs->freq),
		msb(tuneargs->freq),
		lsb(tuneargs->antcap),
		msb(tuneargs->antcap),
		tuneargs->program_id,
	};

	return si468x_cmd_tune_seek_freq(core, CMD_FM_TUNE_FREQ,
					 args, sizeof(args),
					 resp, sizeof(resp));
}
EXPORT_SYMBOL_GPL(si468x_core_cmd_fm_tune_freq);

int si468x_core_cmd_dab_tune_freq(struct si468x_core *core,
				  struct si468x_tune_freq_args *tuneargs)
{
	u8 resp[CMD_DAB_TUNE_FREQ_NRESP];
	u8 args[CMD_DAB_TUNE_FREQ_NARGS] = {
		(tuneargs->injside),
		0,
		0,
		lsb(tuneargs->antcap),
		msb(tuneargs->antcap),
	};
	int i = 0;

	do {
		if (tuneargs->dab_freq_list[i].frequency ==
		    tuneargs->freq) {
			args[1] = i;
			return si468x_cmd_tune_seek_freq(core,
							 CMD_DAB_TUNE_FREQ,
							 args, sizeof(args),
							 resp, sizeof(resp));
		    }
		i++;
	} while (tuneargs->dab_freq_list[i].frequency &&
		 i < SI468X_DAB_MAX_FREQUENCIES);
	return -EINVAL;
}
EXPORT_SYMBOL_GPL(si468x_core_cmd_dab_tune_freq);

int si468x_core_cmd_dab_set_freq_list(struct si468x_core *core,
				      struct si468x_dab_frequency *dab_freq_list,
				      u8 dab_freq_list_length,
				      u32 si468x_dab_max_frequencies)
{
	int  err;
	int  i;
	char *tx_buf;
	u8   resp[CMD_DAB_SET_FREQ_LIST_NRESP];

	tx_buf = kmalloc(CMD_DAB_SET_FREQ_LIST_NARGS + dab_freq_list_length * 4,
			 GFP_KERNEL);
	if (!tx_buf)
		return -ENOMEM;
	tx_buf[0] = dab_freq_list_length;
	tx_buf[1] = 0;
	tx_buf[2] = 0;
	for (i = 0; i < dab_freq_list_length; i++) {
		tx_buf[3 + 4 * i] = cpu_to_le32(dab_freq_list[i].frequency) & 0xff;
		tx_buf[4 + 4 * i] = cpu_to_le32(dab_freq_list[i].frequency >> 8) & 0xff;
		tx_buf[5 + 4 * i] = cpu_to_le32(dab_freq_list[i].frequency >> 16) & 0xff;
		tx_buf[6 + 4 * i] = cpu_to_le32(dab_freq_list[i].frequency >> 24) & 0xff;
	}
	err = si468x_core_send_command(core, CMD_DAB_SET_FREQ_LIST,
				       tx_buf,
				       CMD_DAB_SET_FREQ_LIST_NARGS +
				       dab_freq_list_length * 4,
				       resp, ARRAY_SIZE(resp),
				       SI468X_DEFAULT_TIMEOUT);
	kfree(tx_buf);

	return err;
}
EXPORT_SYMBOL_GPL(si468x_core_cmd_dab_set_freq_list);

int si468x_core_cmd_dab_get_freq_list(struct si468x_core *core,
				      struct si468x_dab_frequency *dab_freq_list,
				      u32 dab_freq_list_length,
				      u32 si468x_dab_max_frequencies)
{
	int err;
	int num_freqs;
	int i, loop;
	u32 si468x_dab_freq;
	char *rx_buf;
	u8       resp[CMD_DAB_GET_FREQ_LIST_NRESP];
	const u8 args[CMD_DAB_GET_FREQ_LIST_NARGS] = {
		0,
	};

	err = si468x_core_send_command(core, CMD_DAB_GET_FREQ_LIST,
				       args, ARRAY_SIZE(args),
				       resp, ARRAY_SIZE(resp),
				       SI468X_DEFAULT_TIMEOUT);
	if (err < 0)
		return err;
	
	num_freqs = resp[4];
	if (num_freqs > si468x_dab_max_frequencies)
			return -EINVAL;
	rx_buf = kmalloc(ARRAY_SIZE(resp) + num_freqs * 4,
			 GFP_KERNEL);
	if (!rx_buf)
		return -ENOMEM;
	err = si468x_core_send_command(core, CMD_DAB_GET_FREQ_LIST,
				       args, ARRAY_SIZE(args),
				       rx_buf,
				       ARRAY_SIZE(resp) + num_freqs * 4,
				       SI468X_DEFAULT_TIMEOUT);
	if (!(err < 0)) {
		if (num_freqs != resp[4]) {
			err = -EINVAL;
			goto out;
		}
		for (i = 0; i < dab_freq_list_length; i++)
			dab_freq_list[i].is_active = false;
		for (i = 0; i < num_freqs; i++) {
			si468x_dab_freq = get_unaligned_le32(rx_buf +
							     ARRAY_SIZE(resp) +
							     4 * i);
			for (loop = 0; loop < dab_freq_list_length; loop++)
				if (si468x_dab_freq ==
				    dab_freq_list[loop].frequency)
					dab_freq_list[loop].is_active = true;
		}
	}
out:
	kfree(rx_buf);

	return err;
}
EXPORT_SYMBOL_GPL(si468x_core_cmd_dab_get_freq_list);

int si468x_core_cmd_agc_status(struct si468x_core *core,
					struct si468x_agc_status_report *report)
{
	int err;
	u8 resp[CMD_GET_AGC_STATUS_NRESP];
	const u8 args[CMD_GET_AGC_STATUS_NARGS] = {
		0,
	};

	if (!report)
		return -EINVAL;

	si468x_core_lock(core);
	err = si468x_core_send_command(core, CMD_GET_AGC_STATUS,
				       args, sizeof(args),
				       resp, ARRAY_SIZE(resp),
				       SI468X_DEFAULT_TIMEOUT);
	si468x_core_unlock(core);
	if (err < 0)
		return err;

	report->vhflna		= resp[14];
	report->vhfcatt		= resp[15];
	report->vhfrattlo	= resp[16];
	report->vhfratthi	= resp[17];
	report->lmhfrtanklo	= resp[18];
	report->lmhfrtankhi	= resp[19];
	report->rfindex		= resp[21];
	report->lmhfcatt	= resp[24];

	return err;
}
EXPORT_SYMBOL_GPL(si468x_core_cmd_agc_status);

struct si468x_core *si468x_core_probe(struct device *dev, int irq,
				      const struct si468x_bus_ops *bus_ops)
{
	int rval;
	struct si468x_core *core;
	struct mfd_cell    *cell;
	struct device_node *node = dev->of_node;
	struct clk         *clk;
	int                cell_num;
	unsigned long      freq;

	core = devm_kzalloc(dev, sizeof(*core), GFP_KERNEL);
	if (!core)
		return ERR_PTR(-ENOMEM);

	core->dev = dev;

	core->si468x_device_info = of_device_get_match_data(dev);
	if (!core->si468x_device_info) {
		dev_err(core->dev, "unknown device model\n");
		return ERR_PTR(-ENODEV);
	}

	core->bus_ops = bus_ops;
	rval = devm_regmap_init_si468x(core);
	if (rval) {
		dev_err(core->dev,
			"Failed to allocate register map: %d\n",
			rval);
		return ERR_PTR(rval);
	}

	atomic_set(&core->is_alive, 0);

	core->gpio_reset = devm_gpiod_get(core->dev, "reset", GPIOD_OUT_HIGH);
	if (IS_ERR(core->gpio_reset)) {
		dev_err(core->dev, "Unable to retrieve reset gpio\n");
		rval = PTR_ERR(core->dev);
		goto free_kfifo;
		}

	core->supplies[0].supply = "vcore";
	core->supplies[1].supply = "va";
	core->supplies[2].supply = "vio";
	core->supplies[3].supply = "vmem";

	rval = devm_regulator_bulk_get(core->dev,
				       ARRAY_SIZE(core->supplies),
				       core->supplies);
	if (rval) {
		dev_err(core->dev, "Failed to get all of the regulators\n");
		return ERR_PTR(rval);
	}

	mutex_init(&core->cmd_lock);
	init_waitqueue_head(&core->command);
	init_waitqueue_head(&core->tuning);

	rval = kfifo_alloc(&core->rds_fifo,
			   SI468X_DRIVER_RDS_FIFO_DEPTH *
			   sizeof(struct v4l2_rds_data),
			   GFP_KERNEL);
	if (rval) {
		dev_err(core->dev, "Could not allocate the FIFO\n");
		return ERR_PTR(rval);
	}

	clk = devm_clk_get(core->dev, NULL);
	if (IS_ERR(clk)) {
		rval = PTR_ERR(clk);
		dev_err(core->dev, "Cannot get clock %d\n", rval);
		goto free_kfifo;
	} else {
		freq = clk_get_rate(clk);
	}

	/* Sanity check */
	if ((freq <  5400000 || freq >  6600000) &&
	    (freq < 10800000 || freq > 13200000) &&
	    (freq < 16800000 || freq > 19800000) &&
	    (freq < 21600000 || freq > 26400000) &&
	    (freq < 27000000 || freq > 46200000) )
		return ERR_PTR(-ERANGE);
	core->power_up_parameters.clk_frequency = freq;
	switch (core->power_up_parameters.clk_frequency) {
	case 37200000:
		core->power_up_parameters.tr_size = 0x0f;
		break;
	case 27000000:
		core->power_up_parameters.tr_size = 0x0a;
		break;
	case 19200000:
		core->power_up_parameters.tr_size = 0x07;
		break;
	case 12288000:
		core->power_up_parameters.tr_size = 0x05;
		break;
	case 6000000:
		core->power_up_parameters.tr_size = 0x04;
		break;
	default:
		core->power_up_parameters.tr_size = 0x00;
		dev_err(core->dev, "Unknown clock frequency, TR_SIZE not set\n");
	}

	if (of_property_read_u32(node, "clock-mode",
				 &core->power_up_parameters.xmode)) {
		core->power_up_parameters.xmode = SI468X_XMODE_CRYSTAL;
		dev_info(core->dev, "Using default clock mode (crystal)\n");
	}
	if (of_property_read_u32(node, "clock-ibias",
				 &core->power_up_parameters.ibias)) {
		core->power_up_parameters.ibias = 0;
		dev_info(core->dev, "Using default clock IBIAS (0)\n");
	}
	if (of_property_read_u32(node, "clock-ctun",
				 &core->power_up_parameters.xcload)) {
		core->power_up_parameters.xcload = 0;
		dev_info(core->dev, "Using default clock CTUN (0)\n");
	}

	mutex_init(&core->rds_drainer_status_lock);
	init_waitqueue_head(&core->rds_read_queue);
	INIT_WORK(&core->rds_fifo_drainer, si468x_core_drain_rds_fifo);

	INIT_WORK(&core->update_service_list,
		  si468x_core_new_digital_service_list);
	mutex_init(&core->digital_service_drainer_status_lock);
	INIT_WORK(&core->update_service_data,
		  si468x_core_new_digital_service_data);

	if (irq) {
		rval = devm_request_threaded_irq(core->dev,
						 irq, NULL,
						 si468x_core_interrupt,
						 IRQF_TRIGGER_FALLING |
						 IRQF_ONESHOT,
						 dev_name(dev), core);
		if (rval < 0) {
			dev_err(core->dev, "Could not request IRQ %d\n",
				irq);
			goto free_kfifo;
		}
		core->irq = irq;
		disable_irq(irq);
		dev_dbg(core->dev, "IRQ requested.\n");

		core->rds_fifo_depth = 20;
	} else {
		dev_err(core->dev,
			"No IRQ number specified\n");
		rval = -EINVAL;
		goto free_kfifo;
	}

	rval = si468x_core_get_revision_info(core);
	if (rval < 0) {
		rval = -ENODEV;
		goto free_kfifo;
	}

	cell_num = 0;

	cell = &core->cells[SI468X_RADIO_CELL];
	cell->name = "si468x-radio";
	cell_num++;

	cell = &core->cells[SI468X_CODEC_CELL];
	cell->name = "si468x-codec";
	cell_num++;

	rval = devm_mfd_add_devices(core->dev,
				    0,
				    core->cells, cell_num,
				    NULL, 0, NULL);
	if (rval < 0)
		goto free_kfifo;

	rval = sysfs_create_group(&core->dev->kobj, &si468x_attr_group);

	if (!rval)
		return core;

free_kfifo:
	kfifo_free(&core->rds_fifo);

	return ERR_PTR(rval);
}
EXPORT_SYMBOL_GPL(si468x_core_probe);

int si468x_core_remove(struct si468x_core *core)
{
	sysfs_remove_group(&core->dev->kobj, &si468x_attr_group);

	si468x_core_pronounce_dead(core);

	disable_irq(core->irq);

	kfifo_free(&core->rds_fifo);

	return 0;
}
EXPORT_SYMBOL_GPL(si468x_core_remove);

void si468x_core_suspend(struct si468x_core *core)
{
}
EXPORT_SYMBOL_GPL(si468x_core_suspend);

void si468x_core_resume(struct si468x_core *core)
{
}
EXPORT_SYMBOL_GPL(si468x_core_resume);

MODULE_AUTHOR("rpi Receiver <rpi-receiver@htl-steyr.ac.at>");
MODULE_DESCRIPTION("API for command exchange for Si468x");
MODULE_LICENSE("GPL");
MODULE_FIRMWARE(SI468X_FIRMWARE_AMHD);
MODULE_FIRMWARE(SI468X_FIRMWARE_FMHD);
MODULE_FIRMWARE(SI468X_FIRMWARE_DAB);
MODULE_FIRMWARE(SI468X_FIRMWARE_PATCH_MINI);
MODULE_FIRMWARE(SI468X_FIRMWARE_PATCH);

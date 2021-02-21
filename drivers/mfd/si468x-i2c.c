// SPDX-License-Identifier: GPL-2.0-only
/*
 * drivers/mfd/si468x-i2c.c -- Core device driver for si468x MFD
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
#include <linux/module.h>

#include <linux/input.h>	/* BUS_I2C */
#include <linux/i2c.h>
#include <linux/interrupt.h>
#include <linux/err.h>

#include <linux/mfd/si468x-core.h>

#define SI468X_MAX_IO_ERRORS		10

static int si468x_smbus_write(struct si468x_core *core,
			      char *buf, int count)
{
	static int io_errors_count;
	struct i2c_client *client = to_i2c_client(core->dev);
	int err;

	err = i2c_master_send(client, buf, count);

	if (err < 0) {
		if (io_errors_count++ > SI468X_MAX_IO_ERRORS)
			si468x_core_pronounce_dead(core);
	} else {
		io_errors_count = 0;
	}

	return err;
}

static int si468x_smbus_read(struct si468x_core *core,
			     char *buf, int count)
{
	static int io_errors_count;
	struct i2c_client *client = to_i2c_client(core->dev);
	int err;

	err = i2c_master_recv(client, buf, count);

	if (err < 0) {
		if (io_errors_count++ > SI468X_MAX_IO_ERRORS)
			si468x_core_pronounce_dead(core);
	} else {
		io_errors_count = 0;
	}

	return err;
}

const struct si468x_bus_ops si468x_i2c_bus_ops = {
	.bustype	= BUS_I2C,
	.write		= si468x_smbus_write,
	.read		= si468x_smbus_read,
};

static int si468x_i2c_probe(struct i2c_client *client,
			    const struct i2c_device_id *id)
{ /* i2c and spi interface: adxl34x.c */
	struct si468x_core *core;
	int err;

	err = i2c_check_functionality(client->adapter,
			I2C_FUNC_I2C);
	if (err < 0) {
		dev_err(&client->dev, "I2C_FUNC_I2C not Supported\n");
		return -EIO;
	}

	core = si468x_core_probe(&client->dev, client->irq,
			    &si468x_i2c_bus_ops);
	if (IS_ERR(core))
		return PTR_ERR(core);

	i2c_set_clientdata(client, core);

	return 0;
}

static int si468x_i2c_remove(struct i2c_client *client)
{
	struct si468x_core *core = i2c_get_clientdata(client);

	si468x_core_remove(core);

	return 0;
}

static int __maybe_unused si468x_i2c_suspend(struct device *dev)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct si468x_core *core = i2c_get_clientdata(client);

	si468x_core_suspend(core);

	return 0;
}

static int __maybe_unused si468x_i2c_resume(struct device *dev)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct si468x_core *core = i2c_get_clientdata(client);

	si468x_core_resume(core);

	return 0;
}

static SIMPLE_DEV_PM_OPS(si468x_i2c_pm, si468x_i2c_suspend,
			 si468x_i2c_resume);

static const struct i2c_device_id si468x_id[] = {
	{ "si4682", SI468X_CHIP_SI4682 },
	{ "si4683", SI468X_CHIP_SI4683 },
	{ "si4684", SI468X_CHIP_SI4684 },
	{ "si4688", SI468X_CHIP_SI4688 },
	{ "si4689", SI468X_CHIP_SI4689 },
	{ },
};
MODULE_DEVICE_TABLE(i2c, si468x_id);

MODULE_DEVICE_TABLE(of, of_si468x_match);

static struct i2c_driver si468x_i2c_driver = {
	.driver		= {
		.name	= "si468x-core",
		.pm = &si468x_i2c_pm,
		.of_match_table	= of_si468x_match,
	},
	.probe		= si468x_i2c_probe,
	.remove         = si468x_i2c_remove,
	.id_table       = si468x_id,
};
module_i2c_driver(si468x_i2c_driver);

MODULE_AUTHOR("rpi Receiver <rpi-receiver@htl-steyr.ac.at>");
MODULE_DESCRIPTION("Si468x AM/FM/DAB core device driver");
MODULE_LICENSE("GPL");
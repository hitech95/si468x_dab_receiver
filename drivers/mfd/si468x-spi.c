// SPDX-License-Identifier: GPL-2.0-only
/*
 * drivers/mfd/si468x-spi.c -- Core device driver for si468x MFD
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

#include <linux/input.h>	/* BUS_SPI */
#include <linux/spi/spi.h>
#include <linux/interrupt.h>
#include <linux/err.h>

#include <linux/mfd/si468x-core.h>

#define SI468X_MAX_IO_ERRORS		10
#define MAX_SPI_FREQ_HZ			10000000

static int si468x_spi_write(struct si468x_core *core,
			      char *buf, int count)
{
	static int io_errors_count;
	struct spi_device *spi = to_spi_device(core->dev);
	int err;

	err = spi_write(spi, buf, count);
	if (err < 0) {
		if (io_errors_count++ > SI468X_MAX_IO_ERRORS)
			si468x_core_pronounce_dead(core);
	} else {
		io_errors_count = 0;
	}

	return (err < 0) ? err : count;
}

static int si468x_spi_read(struct si468x_core *core,
			     char *buf, int count)
{
	static int io_errors_count;
	struct spi_device *spi = to_spi_device(core->dev);
	char *rx_buf;
	int err;

	rx_buf = kmalloc(count + 1, GFP_KERNEL);
	if (!rx_buf)
		return -ENOMEM;
	err = spi_read(spi, rx_buf, count + 1);
	/* discard first byte */
	memcpy(buf, rx_buf + 1, count);
	kfree(rx_buf);
	if (err < 0) {
		if (io_errors_count++ > SI468X_MAX_IO_ERRORS)
			si468x_core_pronounce_dead(core);
	} else {
		io_errors_count = 0;
	}

	return (err < 0) ? err : count;
}

const struct si468x_bus_ops si468x_spi_bus_ops = {
	.bustype	= BUS_SPI,
	.write		= si468x_spi_write,
	.read		= si468x_spi_read,
};

static int si468x_spi_probe(struct spi_device *spi)
{
	struct si468x_core *core;

	/* don't exceed max specified SPI CLK frequency */
	if (spi->max_speed_hz > MAX_SPI_FREQ_HZ) {
		dev_err(&spi->dev, "SPI CLK %d Hz too fast\n", spi->max_speed_hz);
		return -EINVAL;
	}

	core = si468x_core_probe(&spi->dev, spi->irq,
			    &si468x_spi_bus_ops);
	if (IS_ERR(core))
		return PTR_ERR(core);

	spi_set_drvdata(spi, core);

	return 0;
}

static int si468x_spi_remove(struct spi_device *spi)
{
	struct si468x_core *core = spi_get_drvdata(spi);

	si468x_core_remove(core);

	return 0;
}

static int __maybe_unused si468x_spi_suspend(struct device *dev)
{
	struct spi_device *spi = to_spi_device(dev);
	struct si468x_core *core = spi_get_drvdata(spi);

	si468x_core_suspend(core);

	return 0;
}

static int __maybe_unused si468x_spi_resume(struct device *dev)
{
	struct spi_device *spi = to_spi_device(dev);
	struct si468x_core *core = spi_get_drvdata(spi);

	si468x_core_resume(core);

	return 0;
}

static SIMPLE_DEV_PM_OPS(si468x_spi_pm, si468x_spi_suspend,
			 si468x_spi_resume);

static const struct spi_device_id si468x_id[] = {
	{ "si4682", SI468X_CHIP_SI4682 },
	{ "si4683", SI468X_CHIP_SI4683 },
	{ "si4684", SI468X_CHIP_SI4684 },
	{ "si4688", SI468X_CHIP_SI4688 },
	{ "si4689", SI468X_CHIP_SI4689 },
	{ },
};
MODULE_DEVICE_TABLE(spi, si468x_id);

MODULE_DEVICE_TABLE(of, of_si468x_match);

static struct spi_driver si468x_spi_driver = {
	.driver		= {
		.name	= "si468x-core",
		.pm = &si468x_spi_pm,
		.of_match_table	= of_si468x_match,
	},
	.probe		= si468x_spi_probe,
	.remove         = si468x_spi_remove,
	.id_table       = si468x_id,
};
module_spi_driver(si468x_spi_driver);

MODULE_AUTHOR("rpi Receiver <rpi-receiver@htl-steyr.ac.at>");
MODULE_DESCRIPTION("Si468x AM/FM/DAB core device driver");
MODULE_LICENSE("GPL");
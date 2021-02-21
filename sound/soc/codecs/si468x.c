// SPDX-License-Identifier: GPL-2.0-only
/*
 * sound/soc/codecs/si468x.c -- Codec driver for SI468X chips
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
#include <linux/slab.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <linux/regmap.h>
#include <sound/soc.h>
#include <sound/initval.h>

#include <linux/i2c.h>

#include <linux/mfd/si468x-core.h>

enum si468x_digital_io_output_format {
	SI468X_DIGITAL_IO_SLOT_SIZE_SHIFT	= 4,
	SI468X_DIGITAL_IO_SAMPLE_SIZE_SHIFT	= 8,
};

#define SI468X_DIGITAL_IO_OUTPUT_WIDTH_MASK	((0xf << SI468X_DIGITAL_IO_SLOT_SIZE_SHIFT) | \
						  (0x3f << SI468X_DIGITAL_IO_SAMPLE_SIZE_SHIFT))
#define SI468X_DIGITAL_IO_OUTPUT_FORMAT_MASK	(0xf)

enum si468x_daudio_formats {
	SI468X_DAUDIO_MODE_I2S		= (0x0),
	SI468X_DAUDIO_MODE_DSP_A	= (0x6),
	SI468X_DAUDIO_MODE_DSP_B	= (0x7),
	SI468X_DAUDIO_MODE_LEFT_J	= (0x8),
	SI468X_DAUDIO_MODE_RIGHT_J	= (0x9),
};

enum si468x_pcm_format {
	SI468X_SLOT_SIZE_EQU_SAMPLE	= 0,
	SI468X_SLOT_SIZE_8_BIT		= 2,
	SI468X_SLOT_SIZE_16_BIT		= 4,
	SI468X_SLOT_SIZE_20_BIT		= 5,
	SI468X_SLOT_SIZE_24_BIT		= 6,
	SI468X_SLOT_SIZE_32_BIT		= 7,
};

static const struct snd_soc_dapm_widget si468x_dapm_widgets[] = {
	SND_SOC_DAPM_OUTPUT("LOUT"),
	SND_SOC_DAPM_OUTPUT("ROUT"),
};

static const struct snd_soc_dapm_route si468x_dapm_routes[] = {
	{ "Capture", NULL, "LOUT" },
	{ "Capture", NULL, "ROUT" },
};

static int si468x_codec_set_dai_fmt(struct snd_soc_dai *codec_dai,
				    unsigned int fmt)
{
	struct si468x_core *core = dev_get_drvdata(codec_dai->dev->parent);
	int err;
	u16 format = 0;

	if ((fmt & SND_SOC_DAIFMT_MASTER_MASK) != SND_SOC_DAIFMT_CBS_CFS)
		return -EINVAL;

	switch (fmt & SND_SOC_DAIFMT_FORMAT_MASK) {
	case SND_SOC_DAIFMT_DSP_A:
		format |= SI468X_DAUDIO_MODE_DSP_A;
		break;
	case SND_SOC_DAIFMT_DSP_B:
		format |= SI468X_DAUDIO_MODE_DSP_B;
		break;
	case SND_SOC_DAIFMT_I2S:
		format |= SI468X_DAUDIO_MODE_I2S;
		break;
	case SND_SOC_DAIFMT_RIGHT_J:
		format |= SI468X_DAUDIO_MODE_RIGHT_J;
		break;
	case SND_SOC_DAIFMT_LEFT_J:
		format |= SI468X_DAUDIO_MODE_LEFT_J;
		break;
	default:
		return -EINVAL;
	}

	si468x_core_lock(core);

	err = snd_soc_component_write(codec_dai->component,
				      SI468X_PROP_PIN_CONFIG_ENABLE,
				      SI468X_PROP_I2S_ENABLED);
	if (err < 0) {
		dev_err(codec_dai->component->dev, "Pin config failed\n");
		goto out;
	}

	/* This property may only be written before the first tune */
	err = snd_soc_component_update_bits(codec_dai->component,
					    SI468X_PROP_DIGITAL_IO_OUTPUT_FORMAT,
					    SI468X_DIGITAL_IO_OUTPUT_FORMAT_MASK,
					    format);
out:
	si468x_core_unlock(core);

	if (err < 0) {
		dev_err(codec_dai->component->dev, "Failed to set output format\n");
		return err;
	}

	return 0;
}

static int si468x_codec_hw_params(struct snd_pcm_substream *substream,
				  struct snd_pcm_hw_params *params,
				  struct snd_soc_dai *dai)
{
	struct si468x_core *core = dev_get_drvdata(dai->dev->parent);
	int rate, width, period_size, err;

	rate = params_rate(params);
	if (rate < 32000 || rate > 48000) {
		dev_err(dai->component->dev, "Rate: %d is not supported\n", rate);
		return -EINVAL;
	}

	width = params_width(params);
	if (width < 8)
		return -EINVAL;
	if (width > 24)
		width = 24; /* limit to 24 bit */

	if (width == params_physical_width(params))
		period_size = SI468X_SLOT_SIZE_EQU_SAMPLE;
	else
		switch (params_physical_width(params)) {
		case 8:
			period_size = SI468X_SLOT_SIZE_8_BIT;
			break;
		case 16:
			period_size = SI468X_SLOT_SIZE_16_BIT;
			break;
		case 20:
			period_size = SI468X_SLOT_SIZE_20_BIT;
			break;
		case 24:
			period_size = SI468X_SLOT_SIZE_24_BIT;
			break;
		case 32:
			period_size = SI468X_SLOT_SIZE_32_BIT;
			break;
		default:
			return -EINVAL;
		}

	si468x_core_lock(core);

	err = snd_soc_component_write(dai->component, SI468X_PROP_DIGITAL_IO_OUTPUT_SAMPLE_RATE,
			    rate);
	if (err < 0) {
		dev_err(dai->component->dev, "Failed to set sample rate\n");
		goto out;
	}

	/* This property may only be written before the first tune */
	err = snd_soc_component_update_bits(dai->component, SI468X_PROP_DIGITAL_IO_OUTPUT_FORMAT,
				  SI468X_DIGITAL_IO_OUTPUT_WIDTH_MASK,
				  (period_size << SI468X_DIGITAL_IO_SLOT_SIZE_SHIFT) |
				  (width << SI468X_DIGITAL_IO_SAMPLE_SIZE_SHIFT));
	if (err < 0) {
		dev_err(dai->component->dev, "Failed to set DIGITAL_IO_OUTPUT_FORMAT\n");
		goto out;
	}

out:
	si468x_core_unlock(core);

	return err;
}

static const struct snd_soc_dai_ops si468x_dai_ops = {
	.hw_params	= si468x_codec_hw_params,
	.set_fmt	= si468x_codec_set_dai_fmt,
};

static struct snd_soc_dai_driver si468x_dai = {
	.name		= "si468x-codec",
	.capture	= {
		.stream_name	= "Capture",
		.channels_min	= 2,
		.channels_max	= 2,

		.rates = SNDRV_PCM_RATE_32000 |
		SNDRV_PCM_RATE_44100 |
		SNDRV_PCM_RATE_48000,
		.formats = SNDRV_PCM_FMTBIT_S8 |
		SNDRV_PCM_FMTBIT_S16_LE |
		SNDRV_PCM_FMTBIT_S20_3LE |
		SNDRV_PCM_FMTBIT_S24_LE |
		SNDRV_PCM_FMTBIT_S32_LE
	},
	.ops		= &si468x_dai_ops,
};

static int si468x_probe(struct snd_soc_component *component)
{
	struct si468x_core *core = dev_get_drvdata(component->dev->parent);

	snd_soc_component_init_regmap(component, core->regmap_common);

	return 0;
}

static const struct snd_soc_component_driver soc_component_dev_si468x = {
	.probe			= si468x_probe,
	.dapm_widgets		= si468x_dapm_widgets,
	.num_dapm_widgets	= ARRAY_SIZE(si468x_dapm_widgets),
	.dapm_routes		= si468x_dapm_routes,
	.num_dapm_routes	= ARRAY_SIZE(si468x_dapm_routes),
	.idle_bias_on		= 1,
	.use_pmdown_time	= 1,
	.endianness		= 1,
	.non_legacy_dai_naming	= 1,
};

static int si468x_platform_probe(struct platform_device *pdev)
{
	return devm_snd_soc_register_component(&pdev->dev,
				      &soc_component_dev_si468x,
				      &si468x_dai, 1);
}

MODULE_ALIAS("platform:si468x-codec");

static struct platform_driver si468x_platform_driver = {
	.driver		= {
		.name	= "si468x-codec",
	},
	.probe		= si468x_platform_probe,
};
module_platform_driver(si468x_platform_driver);

MODULE_AUTHOR("rpi Receiver <rpi-receiver@htl-steyr.ac.at>");
MODULE_DESCRIPTION("ASoC Si468x AM/FM/DAB codec driver");
MODULE_LICENSE("GPL");
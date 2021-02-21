#!/bin/bash
# Pin configuration property
SI468X_PROP_PIN_CONFIG_ENABLE='0x0800'
# FM Front End Varactor configuration slope */
SI468X_PROP_FM_TUNE_FE_VARM='0x1710'
# FM Front End Varactor configuration intercept */
SI468X_PROP_FM_TUNE_FE_VARB='0x1711'
# Additional configuration options for the front end. */
SI468X_PROP_FM_TUNE_FE_CFG='0x1712'

SYSPATH='/sys/devices/platform/soc/fe804000.i2c/i2c-1/1-0064/si468x_property'

cat /dev/radio0 &
PID_CAT=$!
sleep 2
echo -e "${SI468X_PROP_PIN_CONFIG_ENABLE} 0x8002" | sudo tee "${SYSPATH}"
echo -e "${SI468X_PROP_FM_TUNE_FE_VARM} 0x8600" | sudo tee "${SYSPATH}"
echo -e "${SI468X_PROP_FM_TUNE_FE_VARB} 0x00c9" | sudo tee "${SYSPATH}"
echo -e "${SI468X_PROP_FM_TUNE_FE_CFG} 0x0001" | sudo tee "${SYSPATH}"

for i in $(printf '0x%x\n' $(seq 1 100 6000))
do
	echo -e "${SI468X_PROP_FM_TUNE_FE_VARB} $i" | sudo tee "${SYSPATH}"
	v4l2-ctl -d /dev/radio0 --set-freq=88.80
	sleep 0.2
	v4l2-ctl -d /dev/radio0 --set-freq=104.00
	sleep 0.5
	v4l2-ctl -d /dev/radio0 --get-tuner | grep 'Signal strength'
done

kill "$PID_CAT"
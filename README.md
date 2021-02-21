# SI468x DAB Receiver modules and design files
## To compile modules install kernel headers
```console
sudo apt-get install -y raspberrypi-kernel-headers
```
then run
```console
sudo install.sh
```
and reboot, that's it. Take care of the Device-Tree
### Without a PLL not all MCLK and  BCLK combinations are supported
add the alsa configuration
```console
sudo cp /home/pi/rpi-receiver-linux-rpi-*/asound.conf /etc/
```
## SI468x Firmware
unfortunately the firmware is closed source. You can buy an EVAL Board to get old ones (that is what I did).
Copy files:
```console
sudo cp -r /home/pi/rpi-receiver-linux-rpi-*/si468x /lib/firmware
```
You can copy the files to the connected EEPROM:
```console
echo fm | sudo tee /sys/devices/platform/soc/*.i2c/i2c-1/1-0064/si468x_nvram
```
copies the fm firmware referenced from the devicetree to the nvram
## Tuning
### You can use the v4l2 tools
```console
v4l2-ctl -d /dev/radio0 --set-freq=88.80
```
tunes to FM 88.8MHz.
### Search next station
```console
v4l2-ctl -d /dev/radio0 --freq-seek=dir=1,wrap=1 && v4l2-ctl -d /dev/radio0 --get-freq
```
### Adjusting volume
```console
v4l2-ctl -d /dev/radio0 --set-ctrl volume=63
```
sets the volume and activates the analog output of the SI468x
### Tuning to DAB
simply use a frequency in the DAB Band
```console
v4l2-ctl -d /dev/radio0 --freq-seek=low=187.072,high=3412,spacing=0 && cat /sys/devices/platform/soc/*.i2c/i2c-1/1-0064/si468x_service_list
```
will also print a service list

Service List:
| MHz | Service ID | SubChId | FIC | Strength | Country | Version | started | Label |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 187.072 | 3153 | 2 | 100 | 43 | 10 | 46 | - | ENERGY |
| 187.072 | 989 | 1 | 100 | 43 | 10 | 46 | - | R.Maria |
| 187.072 | 3416 | 10 | 100 | 43 | 10 | 46 | - | WELLE 1 |
| 187.072 | 3154 | 9 | 100 | 43 | 10 | 46 | - | Stephans Klassik |
| 187.072 | 3413 | 12 | 100 | 43 | 10 | 46 | - | Mein Kinderradio |
| 187.072 | 3364 | 8 | 100 | 43 | 10 | 46 | - | ERF Plus | 
| 187.072 | 3370 | 7 | 100 | 43 | 10 | 46 | - | Technikum ONE |
| 187.072 | 3367 | 6 | 100 | 43 | 10 | 46 | - | Rock Antenne |
| 187.072 | 3414 | 3 | 100 | 43 | 10 | 46 | - | j▒.live |
| 187.072 | 3143 | 5 | 100 | 43 | 10 | 46 | - | * 88.6 * |
| 187.072 | 3412 | 0 | 100 | 43 | 10 | 46 | * | ARABELLA RELAX |
| 187.072 | 3411 | 11 | 100 | 43 | 10 | 46 | - | KLASSIK RADIO |

use **MHz** for the **low** and **Service ID** for the **high** argument to tune with `v4l2-ctl`

### Get the dynamic label
```console
cat /sys/devices/platform/soc/*.i2c/i2c-1/1-0064/si468x_dynamic_label
```
## ToDo
  * in Austria DAB is broadcasted on a single band - never tried two bands....
  * HF receiver circuit needs to be tuned - needs time and equipment!
  * Charset is not converted (j▒.live should be jö.live), but I do not know if this makes sense in the module

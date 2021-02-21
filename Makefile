obj-$(CONFIG_SND_SOC)	+= drivers/
obj-$(CONFIG_SND_SOC)	+= sound/

export CONFIG_RADIO_SI468X := m
export CONFIG_SND_SOC_SI468X := m
export CONFIG_MFD_SI468X_CORE := m
export CONFIG_MFD_SI468X_I2C := m
export CONFIG_MFD_SI468X_SPI := m

export CONFIG_SND_SOC_SSM2518 := m

export CONFIG_OPT3001 := m

export INCLUDEDIR :=  $(dir $(abspath $(lastword $(MAKEFILE_LIST))))include
$(info $$INCLUDEDIR is [${INCLUDEDIR}])
LINUXINCLUDE += -I$(INCLUDEDIR)
#CC += -I$(CURDIR)/../rpi-receiver-linux-rpi-4.19.y/include

all: clean compile

compile:
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

install:
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules_install
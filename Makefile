ARDUINO_DIR:=/home/hubbe/lib/arduino-1.8.3

BOARD_TAG=teensy31
USB_TYPE=USB_SERIAL
ARDUINO_LIB_PATH=FOOBAR
ARDUINO_LIBS=EEPROM FastLED SD SPI SerialFlash Snooze i2c_t3
include Teensy.mk

MAKEFILES=Makefile Arduino.mk Common.mk Teensy.mk
SOURCE_FILES=lightsaber.ino star_wars_logo_pov_data.h voice_data.h gpl-3.0.txt \
             blades/*.h buttons/*.h motion/*.h mtp/*.h
CONFIG_FILES=common_presets.h graflex_v1_config.h owk_v2_config.h \
             toy_saber_config.h test_bench_config.h crossguard_config.h \
             StarJedi10Font.h v1_config.h v2_config.h v3_config.h \
             default_config.h

export:
	cd .. && zip -9 lightsaber/lightsaber-`sed <lightsaber/lightsaber.ino -n 's@.*lightsaber.ino,v \([^ ]*\) .*$$@\1@gp'`.zip `for x in $(SOURCE_FILES) $(CONFIG_FILES) $(MAKEFILES); do echo lightsaber/$$x ; done`


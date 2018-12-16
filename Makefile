ARDUINO_DIR:=/home/hubbe/lib/arduino-1.8.3

#BOARD_TAG=Proffieboard-L433CC
BOARD_TAG=teensy31

ifeq ($(strip $(BOARD_TAG)),Proffieboard-L433CC)
  ALTERNATE_CORE_PATH=/home/hubbe/lib/arduino-STM32L4
  USB_TYPE=USB_TYPE_CDC_MSC
  ARDUINO_LIBS=EEPROM SPI Wire
  include Proffieboard.mk
else
  USB_TYPE=USB_SERIAL
  ARDUINO_LIB_PATH=FOOBAR
  ARDUINO_LIBS=EEPROM FastLED SD SPI SerialFlash Snooze i2c_t3
  include Teensy.mk
endif

MKFILES=Makefile Arduino.mk Common.mk Teensy.mk sound/Makefile styles/Makefile
SOURCE_FILES=lightsaber.ino gpl-3.0.txt common/*.h display/*.h functions/*.h \
             blades/*.h buttons/*.h motion/*.h mtp/*.h styles/*.h sound/*.h \
             sound/*.cpp styles/*.cpp
CONFIG_FILES=config/*.h

CPPFLAGS += $(TESTFLAGS)

test:
	(cd styles && $(MAKE) test)
	$(MAKE) clean  BOARD_TAG=Proffieboard-L433CC
	$(MAKE) all TESTFLAGS=-DCONFIG_FILE_TEST=\\\"config/default_proffieboard_config.h\\\" BOARD_TAG=Proffieboard-L433CC
	$(MAKE) clean BOARD_TAG=teensy36
	$(MAKE) all TESTFLAGS=-DCONFIG_FILE_TEST=\\\"config/default_v3_config.h\\\" BOARD_TAG=teensy36
	$(MAKE) clean BOARD_TAG=teensy35
	$(MAKE) all TESTFLAGS=-DCONFIG_FILE_TEST=\\\"config/default_v3_config.h\\\" BOARD_TAG=teensy35
	$(MAKE) clean BOARD_TAG=teensy31
	$(MAKE) all TESTFLAGS=-DCONFIG_FILE_TEST=\\\"config/graflex_v1_config.h\\\" BOARD_TAG=teensy31
	$(MAKE) clean BOARD_TAG=teensy31
	$(MAKE) all TESTFLAGS=-DCONFIG_FILE_TEST=\\\"config/prop_shield_fastled_v1_config.h\\\" BOARD_TAG=teensy31
	$(MAKE) clean BOARD_TAG=teensy31
	$(MAKE) all TESTFLAGS=-DCONFIG_FILE_TEST=\\\"config/owk_v2_config.h\\\" BOARD_TAG=teensy31
	$(MAKE) clean BOARD_TAG=teensy31
	$(MAKE) all TESTFLAGS=-DCONFIG_FILE_TEST=\\\"config/crossguard_config.h\\\" BOARD_TAG=teensy31
	$(MAKE) clean BOARD_TAG=teensy31
	$(MAKE) all TESTFLAGS=-DCONFIG_FILE_TEST=\\\"config/test_bench_config.h\\\" BOARD_TAG=teensy31
	$(MAKE) clean BOARD_TAG=teensy31
	$(MAKE) all TESTFLAGS=-DCONFIG_FILE_TEST=\\\"config/toy_saber_config.h\\\" BOARD_TAG=teensy31
	$(MAKE) clean BOARD_TAG=teensy31
	$(MAKE) all TESTFLAGS=-DCONFIG_FILE_TEST=\\\"config/default_v3_config.h\\\" BOARD_TAG=teensy31

# Check that there are no uncommitted changes
cvstest:
	if [ -d CVS ]; then cvs diff >/dev/null 2>&1; fi

export: cvstest test
	cd .. && zip -9 lightsaber/lightsaber-`sed <lightsaber/lightsaber.ino -n 's@.*lightsaber.ino,v \([^ ]*\) .*$$@\1@gp'`.zip `for x in $(SOURCE_FILES) $(CONFIG_FILES) $(MKFILES) ; do echo lightsaber/$$x ; done`


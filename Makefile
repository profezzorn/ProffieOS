ARDUINO_DIR:=/home/hubbe/lib/arduino-1.8.3

#BOARD_TAG=Proffieboard-L433CC
BOARD_TAG=teensy31

ifeq ($(strip $(BOARD_TAG)),Proffieboard-L433CC)
  ALTERNATE_CORE_PATH=/home/hubbe/lib/arduino-STM32L4
  USB_TYPE=USB_TYPE_CDC_MSC
  ARDUINO_LIBS=SPI Wire
  include Proffieboard.mk
else
  USB_TYPE=USB_SERIAL
  ARDUINO_LIB_PATH=FOOBAR
  ARDUINO_LIBS=FastLED SD SPI SerialFlash Snooze i2c_t3
  include Teensy.mk
endif

CPPFLAGS += $(TESTFLAGS)
DIAGNOSTICS_COLOR_WHEN = auto

style-test:
	(cd styles && $(MAKE) test)

test1:
	$(MAKE) all TESTFLAGS=-DCONFIG_FILE_TEST=\\\"config/default_proffieboard_config.h\\\" BOARD_TAG=Proffieboard-L433CC OBJDIR=test-proffieboard-default

test2:
	$(MAKE) all TESTFLAGS=-DCONFIG_FILE_TEST=\\\"config/default_v3_config.h\\\" BOARD_TAG=teensy36 OBJDIR=test-teensy36-default-v3

test3:
	$(MAKE) all TESTFLAGS=-DCONFIG_FILE_TEST=\\\"config/default_v3_config.h\\\" BOARD_TAG=teensy35 OBJDIR=test-teensy35-default-v3

test4:
	$(MAKE) all TESTFLAGS=-DCONFIG_FILE_TEST=\\\"config/graflex_v1_config.h\\\" BOARD_TAG=teensy31 OBJDIR=test-teensy31-graflex-v1

test5:
	$(MAKE) all TESTFLAGS=-DCONFIG_FILE_TEST=\\\"config/prop_shield_fastled_v1_config.h\\\" BOARD_TAG=teensy31 OBJDIR=test-teensy31-fastled-v1

test6:
	$(MAKE) all TESTFLAGS=-DCONFIG_FILE_TEST=\\\"config/owk_v2_config.h\\\" BOARD_TAG=teensy31 OBJDIR=test-teensy31-owk-v2

test7:
	$(MAKE) all TESTFLAGS=-DCONFIG_FILE_TEST=\\\"config/crossguard_config.h\\\" BOARD_TAG=teensy31 OBJDIR=test-teensy31-crossguard-v3

test8:
	$(MAKE) all TESTFLAGS=-DCONFIG_FILE_TEST=\\\"config/test_bench_config.h\\\" BOARD_TAG=teensy31 OBJDIR=test-teensy31-test-v3

test9:
	$(MAKE) all TESTFLAGS=-DCONFIG_FILE_TEST=\\\"config/toy_saber_config.h\\\" BOARD_TAG=teensy31  OBJDIR=test-teensy31-toy-v3

testA:
	$(MAKE) all TESTFLAGS=-DCONFIG_FILE_TEST=\\\"config/default_v3_config.h\\\" BOARD_TAG=teensy31 OBJDIR=test-teensy31-default-v3

test: style-test test1 test2 test3 test4 test5 test6 test7 test8 test9 testA
	@echo Tests pass

# Check that there are no uncommitted changes
cvstest:
	if [ -d CVS ]; then cvs diff >/dev/null 2>&1; fi

export: cvstest test
	cd .. && zip -9 lightsaber/lightsaber-`sed <lightsaber/lightsaber.ino -n 's@.*lightsaber.ino,v \([^ ]*\) .*$$@\1@gp'`.zip `cd lightsaber && cvs status 2>/dev/null | sed -n 's@.*Repository.*/cvs/lightsaber/\(.*\),v@\1@gp' | grep -v 'lightsaber/doc' | grep -v '.cvsignore'`


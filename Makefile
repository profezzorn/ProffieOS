
ARDUINO_DIR:=/home/hubbe/lib/arduino-1.8.15
ARDUINO15_DIR:=$(HOME)/.arduino15/
OBJDIR=build

PBV1_FQBN=proffieboard:stm32l4:Proffieboard-L433CC:usb=cdc,dosfs=sdspi,speed=80,opt=os
PBV2_FQBN=proffieboard:stm32l4:ProffieboardV2-L433CC:usb=cdc,dosfs=sdspi,speed=80,opt=os
PBV3_FQBN=proffieboard:stm32l4:ProffieboardV3-L452RE:usb=cdc,dosfs=sdmmc1,speed=80,opt=os
TEENSY31_FQBN=teensy:avr:teensy31:usb=serial,speed=96,opt=oslto,keys=en-us
TEENSY35_FQBN=teensy:avr:teensy35:usb=serial,speed=120,opt=o2std,keys=en-us
TEENSY36_FQBN=teensy:avr:teensy36:usb=serial,speed=180,opt=o2std,keys=en-us
TEENSY41_FQBN=teensy:avr:teensy41:usb=serial,speed=600,opt=o2std,keys=en-us

# Select one
FQBN=$(PBV1_FQBN)

TESTSUBDIR=NOTUSED

all:
	mkdir build
	$(ARDUINO_DIR)/arduino-builder -compile -hardware $(ARDUINO_DIR)/hardware -hardware $(ARDUINO15_DIR)/packages -hardware /home/hubbe/Arduino/hardware -tools $(ARDUINO_DIR)/tools-builder -tools $(ARDUINO_DIR)/hardware/tools/avr -tools $(ARDUINO15_DIR)/packages -fqbn=$(FQBN) -vid-pid=1209_6668 -build-path build -warnings=more -verbose ProffieOS.ino

CPPFLAGS += $(TESTFLAGS)
DIAGNOSTICS_COLOR_WHEN = auto

style-test:
	(cd styles && $(MAKE) test)

common-test:
	(cd common && $(MAKE) test)

blades-test:
	(cd blades && $(MAKE) test)

sound-test:
	(cd sound && $(MAKE) test)

buttons-test:
	(cd buttons && $(MAKE) test)

display-test:
	(cd display && $(MAKE) test)

subtest:
	rm -r test-$(TESTSUBDIR) || :
	mkdir test-$(TESTSUBDIR)
	mkdir test-$(TESTSUBDIR)/ProffieOS
	tar cf - `git ls-files` | ( cd test-$(TESTSUBDIR)/ProffieOS && tar xf - )
	echo '#define CONFIG_FILE "$(CONFIG_FILE)"' >test-$(TESTSUBDIR)/ProffieOS/ProffieOS.ino
	sed <ProffieOS.ino 's@#define.*CONFIG_FILE.*@@g' >>test-$(TESTSUBDIR)/ProffieOS/ProffieOS.ino
	( cd test-$(TESTSUBDIR)/ProffieOS && $(MAKE) )

test1:
	$(MAKE) subtest CONFIG_FILE=config/default_proffieboard_config.h FQBN=$(PBV1_FQBN) TESTSUBDIR=proffieboard-default

test1V:
	$(MAKE) subtest CONFIG_FILE=config/proffieboard_v1_verification_config.h FQBN=$(PBV1_FQBN) TESTSUBDIR=proffieboard-v1-verification

test2V:
	$(MAKE) subtest CONFIG_FILE=config/proffieboard_v2_verification_config.h FQBN=$(PBV2_FQBN) TESTSUBDIR=proffieboard-v2-verification

test3V:
	$(MAKE) subtest CONFIG_FILE=config/proffieboard_v2_verification_config2.h FQBN=$(PBV2_FQBN) TESTSUBDIR=proffieboard-v2-verification2

test2:
	$(MAKE) subtest CONFIG_FILE=config/default_v3_config.h FQBN=$(TEENSY36_FQBN) TESTSUBDIR=teensy36-default-v3

test3:
	$(MAKE) subtest CONFIG_FILE=config/default_v3_config.h FQBN=$(TEENSY35_FQBN) TESTSUBDIR=teensy35-default-v3

test4:
	$(MAKE) subtest CONFIG_FILE=config/graflex_v1_config.h FQBN=$(TEENSY31_FQBN) TESTSUBDIR=teensy31-graflex-v1

test5:
	$(MAKE) subtest CONFIG_FILE=config/prop_shield_fastled_v1_config.h FQBN=$(TEENSY31_FQBN) TESTSUBDIR=teensy31-fastled-v1

test6:
	$(MAKE) subtest CONFIG_FILE=config/owk_v2_config.h FQBN=$(TEENSY31_FQBN) TESTSUBDIR=teensy31-owk-v2

test7:
	$(MAKE) subtest CONFIG_FILE=config/crossguard_config.h FQBN=$(TEENSY31_FQBN) TESTSUBDIR=teensy31-crossguard-v3

test8:
	$(MAKE) subtest CONFIG_FILE=config/test_bench_config.h FQBN=$(TEENSY31_FQBN) TESTSUBDIR=teensy31-test-v3

test9:
	$(MAKE) subtest CONFIG_FILE=config/toy_saber_config.h FQBN=$(TEENSY31_FQBN)  TESTSUBDIR=teensy31-toy-v3

testA:
	$(MAKE) subtest CONFIG_FILE=config/default_v3_config.h FQBN=$(TEENSY31_FQBN) TESTSUBDIR=teensy31-default-v3

testB:
	$(MAKE) subtest CONFIG_FILE=config/td_proffieboard_config.h FQBN=$(PBV1_FQBN) TESTSUBDIR=proffieboard-td


testC:
	$(MAKE) subtest CONFIG_FILE=config/blaster_v3_config.h FQBN=$(TEENSY31_FQBN) TESTSUBDIR=teensy31-blaster

testV3V:
	$(MAKE) subtest CONFIG_FILE=config/proffieboard_v3_verification_config.h FQBN=$(PBV3_FQBN) TESTSUBDIR=proffieboard-v3-verification

testD:
	$(MAKE) subtest CONFIG_FILE=config/teensy_audio_shield_micom.h FQBN=$(TEENSY41_FQBN) TESTSUBDIR=teensy41-micom


posixtests: style-test common-test blades-test sound-test buttons-test display-test

test: posixtests test1 test2 test3 test4 test5 test6 test7 test8 test9 testA testB testC test1V test2V test3V testV3V testD
	@echo Tests pass

# Check that there are no uncommitted changes
up-to-date-test:
	if [ -d CVS ]; then cvs diff >/dev/null 2>&1; fi
	if [ -d .git ]; then git diff-index --quiet HEAD -- ; fi

export: up-to-date-test test
	if [ -d CVS ]; then cd .. && zip -9 ProffieOS/ProffieOS-`sed <ProffieOS/ProffieOS.ino -n 's@.*ProffieOS.ino,v \([^ ]*\) .*$$@\1@gp'`.zip `cd ProffieOS && cvs status 2>/dev/null | sed -n 's@.*Repository.*/cvs/ProffieOS/\(.*\),v@\1@gp' | grep -v 'ProffieOS/doc' | grep -v '.cvsignore'` ; fi
	if [ -d .git ]; then DIR=`pwd` ; VERSION=`git describe --tags --always --dirty --broken` ; echo "Exporting $$VERSION" ; rm -rvf /tmp/exporttmp || : ; mkdir -p /tmp/exporttmp/ProffieOS && cp -r . /tmp/exporttmp/ProffieOS/ && sed <ProffieOS.ino "s@const char version.*@const char version[] = \"$$VERSION\";@g" >/tmp/exporttmp/ProffieOS/ProffieOS.ino && ( cd /tmp/exporttmp && zip -MM -9 $$DIR/ProffieOS-$$VERSION.zip ProffieOS `cd $$DIR && git ls-files | grep -v doc/ | sed 's@^@ProffieOS/@'` ) && echo "Ok, finished exporting $$VERSION" ; fi

export-dirty:
	if [ -d CVS ]; then cd .. && zip -9 ProffieOS/ProffieOS-`sed <ProffieOS/ProffieOS.ino -n 's@.*ProffieOS.ino,v \([^ ]*\) .*$$@\1@gp'`.zip `cd ProffieOS && cvs status 2>/dev/null | sed -n 's@.*Repository.*/cvs/ProffieOS/\(.*\),v@\1@gp' | grep -v 'ProffieOS/doc' | grep -v '.cvsignore'` ; fi
	if [ -d .git ]; then DIR=`pwd` ; VERSION=`git describe --tags --always --dirty --broken` ; echo "Exporting $$VERSION" ; rm -rvf /tmp/exporttmp || : ; mkdir -p /tmp/exporttmp/ProffieOS && cp -r . /tmp/exporttmp/ProffieOS/ && sed <ProffieOS.ino "s@const char version.*@const char version[] = \"$$VERSION\";@g" >/tmp/exporttmp/ProffieOS/ProffieOS.ino && ( cd /tmp/exporttmp && zip -MM -9 $$DIR/ProffieOS-$$VERSION.zip ProffieOS `cd $$DIR && git ls-files | grep -v doc/ | sed 's@^@ProffieOS/@'` ) && echo "Ok, finished exporting $$VERSION" ; fi

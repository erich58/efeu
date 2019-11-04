TOP=../../../..
include $(TOP)/Setup.mak
include $(CFG)/EFEU.mak

prereq::
	$(MAKE) -C ../efm

LDFLAGS += -lefm -lm -ldl

include $(CFG)/hcfg.mak
include $(CFG)/Library.mak
include $(CFG)/ApplFiles.mak
include $(CFG)/MagicFiles.mak

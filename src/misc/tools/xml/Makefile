# :de: Einfache Hilfsprogramme

TOP=../../../..
include $(TOP)/Setup.mak
include $(CFG)/EFEU.mak

prereq::
	$(MAKE) -C $(TOP)/src/base/lib/efm

CFLAGS += -lefm
include $(CFG)/SimplePrograms.mak

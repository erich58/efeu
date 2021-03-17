# :de: Einfache Hilfsprogramme

TOP=../../../..
include $(TOP)/Setup.mak
include $(CFG)/EFEU.mak

prereq::
	make -C $(TOP)/src/base/lib/efm

CPPFLAGS += -DEFEUCFG
LDFLAGS += -lefm
include $(CFG)/SimplePrograms.mak

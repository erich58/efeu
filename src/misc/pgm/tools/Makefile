# :de: Einfache Hilfsprogramme

TOP=../../../..
include $(TOP)/Setup.mak
include $(CFG)/EFEU.mak

prereq::
	make -C $(TOP)/src/base/lib

LDFLAGS += -liorl -lefx -lefm -lm

include $(CFG)/SimplePrograms.mak

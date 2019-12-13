# :*: data cube client
# :de: Datenwürfelclient

TOP=../../..
include $(TOP)/Setup.mak
include $(CFG)/EFEU.mak

prereq::
	$(MAKE) -C $(TOP)/src/base

LDFLAGS += -liorl -lefx -lefm -lm
include $(CFG)/SimplePrograms.mak


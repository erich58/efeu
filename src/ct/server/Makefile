# :*: data cube client
# :de: Datenwürfelclient

TOP=../../..
NAME=mdmapd
include $(TOP)/Setup.mak
include $(CFG)/EFEU.mak

prereq::
	$(MAKE) -C $(TOP)/src/base

LDFLAGS += -lmd -lefmath -lefx -lefm -lm
include $(CFG)/ComplexProgram.mak

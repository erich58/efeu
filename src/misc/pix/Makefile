# :*: basic libraries
# :de: Basisbibliothek

TOP=../../..
include $(TOP)/Setup.mak
include $(CFG)/EFEU.mak

prereq::
	$(MAKE) -C $(TOP)/src/base/lib/efx

LDFLAGS += -lefm -lefx

include $(CFG)/Library.mak
include $(CFG)/ApplFiles.mak

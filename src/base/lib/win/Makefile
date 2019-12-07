# :*: curses tools
# :de: Fensterhilfsprogramme

TOP=../../../..
include $(TOP)/Setup.mak
include $(CFG)/EFEU.mak

prereq::
	$(MAKE) -C ../efx

LDFLAGS += -lefx -lefm -lncurses

include $(CFG)/Library.mak
include $(CFG)/ApplFiles.mak

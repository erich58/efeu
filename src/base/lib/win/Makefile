# :*: curses tools
# :de: Fensterhilfsprogramme

NAME=efwin
TOP=../../../..
include $(TOP)/Setup.mak
include $(CFG)/EFEU.mak

prereq::
	make -C ../efx

LDFLAGS += -lefx -lefm -lncurses

include $(CFG)/Library.mak
include $(CFG)/ApplFiles.mak

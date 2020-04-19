# :*: extension library
# :de: Erweiterungsbibliothek

TOP=../../../..
include $(TOP)/Setup.mak
include $(CFG)/EFEU.mak

prereq::
	make -C ../efm

LDFLAGS += -lefm -lm -ldl
TST_CFLAGS += -I ../efm

include $(CFG)/hcfg.mak
include $(CFG)/Library.mak
include $(CFG)/MagicFiles.mak

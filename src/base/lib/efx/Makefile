# :*: extension library
# :de: Erweiterungsbibliothek

TOP=../../../..
include $(TOP)/Setup.mak
include $(CFG)/EFEU.mak

prereq::
	make -C ../efm

LDFLAGS += -lefm -lm -ldl
EXT_LIB = $(LIB)/esh
EXT_LDFLAGS = -lefx -lutest
TST_CFLAGS += -I ../efm -D'EXT_LIB="$(EXT_LIB)"'

include $(CFG)/hcfg.mak
include $(CFG)/Library.mak
include $(CFG)/MagicFiles.mak

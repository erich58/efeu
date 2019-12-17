# :*: data cube library
# :de: Datenmatrix

TOP=../../../..
include $(TOP)/Setup.mak
include $(CFG)/EFEU.mak

prereq::
	make -C ../efx

LDFLAGS += -lefx -lefm -ldl -lm

include $(CFG)/Library.mak
include $(CFG)/ApplFiles.mak
include $(CFG)/MagicFiles.mak

# :*: edb commands
# :de: edb  Kommandos

TOP=../../../..
include $(TOP)/Setup.mak
include $(CFG)/EFEU.mak
include $(CFG)/ApplFiles.mak

prereq::
	make -C ../../scripts
	make -C ../../lib

LDFLAGS += -lefmath -liorl -lmd -lefx -lefm -ldl -lm
TST_CFLAGS = '-DBIN="$(BIN)"'
TST_CFLAGS += -I ../../lib/efm -I ../../lib/efx

include $(CFG)/ComplexProgram.mak

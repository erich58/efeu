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
CFLAGS += -I time -I count
TST_CFLAGS += -I ../../lib/efm -I ../../lib/efx

include $(CFG)/ComplexProgram.mak

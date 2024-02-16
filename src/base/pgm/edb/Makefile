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

include $(CFG)/SimplePrograms.mak
include $(CFG)/Scripts.mak

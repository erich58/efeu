TOP=../../../..
include $(TOP)/Setup.mak
include $(CFG)/EFEU.mak
include $(CFG)/ApplFiles.mak

prereq::
	make -C ../DocBuf

CPPFLAGS += -I../include
LDFLAGS += -L../lib -lDocBuf
LDFLAGS += -lefmath -liorl -lmd -lefx -lefm -ldl -lm

include $(CFG)/SimplePrograms.mak


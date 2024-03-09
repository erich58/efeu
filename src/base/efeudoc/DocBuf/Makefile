TOP=../../../..
include $(TOP)/Setup.mak
include $(CFG)/EFEU.mak

prereq::
	make -C $(TOP)/src/base/lib

EFEU_INCLUDE = ../include
CPPFLAGS += -I$(EFEU_INCLUDE)
TST_CFLAGS += -I$(TOP)/src/base/lib/efm

LDFLAGS += -lefmath -liorl -lmd -lefx -lefm -ldl -lm

StaticLibrary = ../lib/lib$(NAME).a
include $(CFG)/Library.mak

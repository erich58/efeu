# :*: basic libraries
# :de: Basisbibliothek

TOP=../../../..
include $(TOP)/Setup.mak
include $(CFG)/EFEU.mak

CFLAGS += -DSB_REALLOC=lrealloc
LDFLAGS += -ldl
TST_LDFLAGS += -lutil

include $(CFG)/hcfg.mak
include $(CFG)/Library.mak

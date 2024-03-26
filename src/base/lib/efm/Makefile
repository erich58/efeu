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

EDOC=$(BIN)/efeudoc

ifneq ($(EDOC),)

doc::
	PATH=$(BIN):$$PATH $(EDOC) --pdf -o intro.pdf intro.doc
else

doc::
	@echo "efeudoc currently not availabel"

endif

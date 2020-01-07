# :de: Einfache Hilfsprogramme

TOP=../../../..
include $(TOP)/Setup.mak
include $(CFG)/EFEU.mak

prereq::
	make -C $(TOP)/src/base/lib/efm

LDFLAGS += -lefm
include $(CFG)/SimplePrograms.mak
include $(CFG)/Scripts.mak

# Extra Symlinks

LINKS= $(BIN)/u2d $(LIB)/efeu/help/u2d.hlp $(LIB)/efeu/de/help/u2d.hlp

all:: $(LINKS)

clean::
	rm -f $(LINKS)

$(BIN)/u2d:
	rm -f $@; cd $(@D) && ln -fs d2u u2d

$(LIB)/efeu/help/u2d.hlp $(LIB)/efeu/de/help/u2d.hlp:
	rm -f $@; cd $(@D) && ln -fs d2u.hlp u2d.hlp

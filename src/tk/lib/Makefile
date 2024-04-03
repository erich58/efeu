# :de: TCL/TK-interface

TOP=../../..
NAME=ETK
include $(TOP)/Setup.mak

EFEU_INCLUDE = $(INC)/GUI
include $(CFG)/EFEU.mak

prereq::
	make -C ../config

../.build/tklib.mak:: prereq

include ../.build/tklib.mak

TK_CONFIG= $(EFEU_INCLUDE)/tk_config.h

LDFLAGS += -lefx -lefm -lreadline -lncurses
TST_CFLAGS += -I $(TOP)/src/base/lib/efm
TST_CFLAGS += -I $(TOP)/src/base/lib/efx

files:: $(TK_CONFIG)

clean::
	rm -f $(TK_CONFIG)

$(TK_CONFIG)::
	@mkdir -p $(@D)
	$(BIN)/efeu-config -t $(TOP) update tk_config.hcfg $@

all:: sanity-check

include $(CFG)/Library.mak

ESH_HDR = $(wildcard *.hdr)
FILES += $(patsubst %,$(LIB)/esh/%,$(notdir $(ESH_HDR)))

all:: $(FILES)

$(LIB)/esh/%.hdr : %.hdr
	@mkdir -p $(@D)
	cp $< $@

clean::
	rm -rf $(BUILD) $(FILES)

# sanity-check

sanity-check:: files

sanity-check:: $(BUILD)/tcl.tst

sanity-check::
	$(BUILD)/tcl.tst 2> /dev/null

$(BUILD)/tcl.tst : tcl.tst.c
	@mkdir -p $(@D)
	$(CC) -o $@ $(CPPFLAGS) $< $(LDFLAGS) -lutest

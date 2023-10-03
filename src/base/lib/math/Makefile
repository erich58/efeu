# :*: mathematican functions
# :de: Mathematische Hilfsfunktionen

TOP=../../../..
NAME=efmath
include $(TOP)/Setup.mak

EFEU_INCLUDE = $(INC)/Math
include $(CFG)/EFEU.mak

prereq::
	make -C ../md

LDFLAGS += -lmd -lefx -lefm -ldl -lm
TST_CFLAGS += -I ../efm

include $(CFG)/Library.mak
include $(CFG)/ApplFiles.mak

# TimeSeries esh header

LIB_TS = $(LIB)/esh/TimeSeries
SRC_HDR = $(wildcard *.hdr */*.hdr)
TS_HDR = $(patsubst %.hdr,$(LIB_TS)/%,$(notdir $(SRC_HDR)))

files:: $(TS_HDR)

clean::
	rm -f $(TS_HDR)

$(LIB_TS)/% : %.hdr
	@mkdir -p $(@D)
	cp $< $@

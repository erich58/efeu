include Setup.mak

EFEU_INCLUDE = $(INC)/Math
include $(CFG)/EFEU.mak

LDFLAGS += -lmd -lefx -lefm -ldl -lm

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

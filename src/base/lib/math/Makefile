include Setup.mak

EFEU_INCLUDE = $(INC)/Math
include $(CFG)/EFEU.mak

LIBRARY = $(LIB)/lib$(NAME).so

LDFLAGS += -lmd -lefx -lefm -ldl -lm

all:: $(LIBRARY)

clean::
	rm -f $(LIBRARY)

$(LIBRARY): $(OBJ)
	@mkdir -p $(@D)
	$(CC) -shared -Wl,-soname,lib$(NAME).so -o $@ $(OBJ) $(LDFLAGS)

include $(CFG)/ApplFiles.mak

# TimeSeries esh header

LIB_TS = $(LIB)/esh/TimeSeries
SRC_HDR = $(wildcard *.hdr */*.hdr)

files:: $(patsubst %.hdr,$(LIB_TS)/%,$(notdir $(SRC_HDR)))

$(LIB_TS)/% : %.hdr
	@mkdir -p $(@D)
	cp $< $@

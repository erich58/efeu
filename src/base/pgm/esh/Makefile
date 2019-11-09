TOP=../../../..
include $(TOP)/Setup.mak
include $(CFG)/EFEU.mak

prereq::
	$(MAKE) -C ../../scripts
	$(MAKE) -C ../../lib

LDFLAGS += -lefmath -liorl -lmd -lefx -lefm -ldl -lm
PGM = $(patsubst %.c,$(BIN)/%,$(notdir $(SRC_C)))

all:: $(PGM)

clean::
	rm -f $(PGM)

$(BIN)/% : %.c
	@mkdir -p $(@D)
	$(CC) -o $@ $(CPPFLAGS) $(CFLAGS) $< $(LDFLAGS)


# Scripts and extra files

SRC_ESH = $(wildcard script/*.esh)
SRC_HDR = $(wildcard mksource/files/*.hdr)
SRC_MS  = $(wildcard mksource/files/*.ms)

FILES = $(patsubst %.esh,$(BIN)/%,$(notdir $(SRC_ESH)))
FILES += $(patsubst %,$(LIB)/esh/%,$(notdir $(SRC_HDR)))
FILES += $(patsubst %,$(LIB)/mksource/%,$(notdir $(SRC_MS)))

all:: $(FILES)

$(BIN)/% : script/%.esh
	$(BIN)/efeuscript -e -c esh $< $@

$(LIB)/esh/%.hdr : mksource/files/%.hdr
	@mkdir -p $(@D)
	cp $< $@

$(LIB)/mksource/%.ms : mksource/files/%.ms
	@mkdir -p $(@D)
	cp $< $@

# compose file

include $(BUILD)/TypeDef.rule

$(BUILD)/TypeDef.rule: mksource/files/type/TypeDef.msx
	$(BIN)/compose-file -r $< $(LIB)/mksource/TypeDef.ms > $@

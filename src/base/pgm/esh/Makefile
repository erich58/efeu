# :*: esh, mksource and scripts
# :de: esh, mksource und Scripts

TOP=../../../..
include $(TOP)/Setup.mak
include $(CFG)/EFEU.mak
include $(CFG)/ApplFiles.mak
include $(CFG)/Scripts.mak

prereq::
	make -C ../../scripts
	make -C ../../lib

LDFLAGS += -lefmath -liorl -lmd -lefx -lefm -ldl -lm
TST_CFLAGS += -D 'SRC="$(shell pwd)"'

include $(CFG)/SimplePrograms.mak

# Extra files

SRC_HDR = $(wildcard mksource/files/*.hdr)
SRC_MS  = $(wildcard mksource/files/*.ms)

FILES = $(patsubst %,$(LIB)/esh/%,$(notdir $(SRC_HDR)))
FILES += $(patsubst %,$(LIB)/mksource/%,$(notdir $(SRC_MS)))

all:: $(FILES)

$(LIB)/esh/%.hdr : mksource/files/%.hdr
	@mkdir -p $(@D)
	cp $< $@

$(LIB)/mksource/%.ms : mksource/files/%.ms
	@mkdir -p $(@D)
	cp $< $@

# compose file

COMPOSE = $(LIB)/mksource/TypeDef.ms

all:: $(COMPOSE)

clean::
	rm -f $(COMPOSE)
	rm -rf $(BUILD)

$(COMPOSE): $(BUILD)/TypeDef.make
	$(MAKE) -f $< all

$(BUILD)/TypeDef.make: mksource/files/type/TypeDef.msx
	@mkdir -p $(BUILD)
	$(BIN)/compose-file -r $< $(COMPOSE) > $@

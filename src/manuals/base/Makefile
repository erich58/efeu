# :*: manpages to the libraries
# :de: Handbücher zu den Programmbibliotheken
TOP=../../..
include $(TOP)/Setup.mak
include $(CFG)/Config.mak

MANLIST= efm
DOCLIST= efm iorl md

# Handbuchseiten

SDIRS = $(patsubst %,$(SRC)/base/lib/%,$(MANLIST))
VPATH = $(shell find $(SDIRS) -depth -type d)
SRC_C = $(shell find $(SDIRS) -type f -name '*.c')
SRC_H = $(shell find $(SDIRS) -type f -name '*.h')

FILES = $(patsubst %.c,$(MAN)/de/man3/%.3.gz,$(notdir $(SRC_C))) 
FILES += $(patsubst %.h,$(MAN)/de/man7/%.7.gz,$(notdir $(SRC_H)))
FILES += $(patsubst %,$(DOC)/de/html/%/index.html,$(DOCLIST))
FILES += $(patsubst %,$(DOC)/de/pdf/%.pdf,$(DOCLIST))

HTML_DIRS = $(patsubst %,$(DOC)/de/html/%,$(DOCLIST))

doc:: $(FILES)

clean::
	rm -rf $(FILES) $(HTML_DIRS)

$(MAN)/de/man3/%.3.gz: %.c
	@mkdir -p $(@D)
	LANG=de_AT $(BIN)/src2doc $< | $(BIN)/efeudoc -Tman - | gzip > $@
	$(BIN)/src2doc -a $< | $(BIN)/efeu-symlinks -s .3.gz $@

$(MAN)/de/man7/%.7.gz: %.h
	@mkdir -p $(@D)
	LANG=de_AT $(BIN)/src2doc $< | $(BIN)/efeudoc -Tman - | gzip > $@
	$(BIN)/src2doc -a $< | $(BIN)/efeu-symlinks -s .7.gz $@

$(DOC)/de/pdf/%.pdf: $(SRC)/base/lib/%/main.doc
	@mkdir -p $(BUILD) $(@D)
	LANG=de_AT $(BIN)/efeudoc -M $(BUILD)/$(@F) --pdf -o $@ $<

-include $(patsubst %,$(BUILD)/%.pdf,$(DOCLIST))

$(DOC)/de/html/%/index.html: $(SRC)/base/lib/%/main.doc
	@mkdir -p $(BUILD) $(@D)
	LANG=de_AT $(BIN)/efeudoc -M $(BUILD)/$(notdir $(@D)) -h -o $(@D) $<

-include $(patsubst %,$(BUILD)/%,$(DOCLIST))

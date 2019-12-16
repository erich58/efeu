# :*: manpages to the libraries
# :de: Handbücher zu den Programmbiblitheken
TOP=../../..
include $(TOP)/Setup.mak
include $(CFG)/Config.mak

SDIRS=$(SRC)/base/lib/efm

VPATH = $(shell find $(SDIRS) -depth -type d)
SRC_C = $(shell find $(SDIRS) -type f -name '*.c')
SRC_H = $(shell find $(SDIRS) -type f -name '*.h')

FILES = $(patsubst %.c,$(MAN)/de/man3/%.3.gz,$(notdir $(SRC_C)))
FILES += $(patsubst %.h,$(MAN)/de/man7/%.7.gz,$(notdir $(SRC_H)))

doc:: $(FILES)

clean::
	rm -f $(FILES)

$(MAN)/de/man3/%.3.gz: %.c
	@mkdir -p $(@D)
	LANG=de_AT $(BIN)/src2doc $< | $(BIN)/efeudoc -Tman - | gzip > $@

$(MAN)/de/man7/%.7.gz: %.h
	@mkdir -p $(@D)
	LANG=de_AT $(BIN)/src2doc $< | $(BIN)/efeudoc -Tman - | gzip > $@

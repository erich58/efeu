# :*: man pages for installed files
# :de: Handbucheinträge aus installierten Dateien generieren

TOP=../../..
include $(TOP)/Setup.mak
include $(CFG)/Config.mak

SMH = $(wildcard $(LIB)/shmkmf/*.smh)
MS = $(wildcard $(LIB)/mksource/*.ms)

FILES = $(patsubst %.smh,$(MAN)/man7/%.7.gz,$(notdir $(SMH))) 
FILES += $(patsubst %.smh,$(MAN)/de/man7/%.7.gz,$(notdir $(SMH))) 
FILES += $(patsubst %.ms,$(MAN)/de/man7/%.7.gz,$(notdir $(MS)))

doc:: $(FILES)

clean::
	rm -rf $(FILES)

$(MAN)/man7/%.7.gz: $(LIB)/shmkmf/%.smh
	@mkdir -p $(@D)
	LANG=en_US $(BIN)/src2doc $< | $(BIN)/efeudoc -Tman - | gzip > $@

$(MAN)/de/man7/%.7.gz: $(LIB)/shmkmf/%.smh
	@mkdir -p $(@D)
	LANG=de_AT $(BIN)/src2doc $< | $(BIN)/efeudoc -Tman - | gzip > $@

$(MAN)/de/man7/%.7.gz: $(LIB)/mksource/%.ms
	@mkdir -p $(@D)
	LANG=de_AT $(BIN)/src2doc $< | $(BIN)/efeudoc -Tman - | gzip > $@

# :*: man pages for installed files
# :de: Handbucheinträge aus installierten Dateien generieren

TOP=../../..
include $(TOP)/Setup.mak
include $(CFG)/Config.mak

SMH = $(wildcard $(LIB)/shmkmf/*.smh)
MS = $(wildcard $(LIB)/mksource/*.ms)

FILES = $(patsubst %,$(MAN)/man7/%.7.gz,$(notdir $(SMH))) 
FILES += $(patsubst %,$(MAN)/de/man7/%.7.gz,$(notdir $(SMH))) 
FILES += $(patsubst %.ms,$(MAN)/de/man7/%.7.gz,$(notdir $(MS)))

doc:: $(FILES)

clean::
	rm -rf $(FILES)

$(MAN)/man7/%.7.gz: $(LIB)/shmkmf/%
	@mkdir -p $(@D)
	LANG=en_US $(BIN)/src2doc $< | $(BIN)/efeudoc -Tman - | gzip > $@
	$(BIN)/src2doc -a $< | $(BIN)/efeu-symlinks -s .smh.7.gz $@

$(MAN)/de/man7/%.7.gz: $(LIB)/shmkmf/%
	@mkdir -p $(@D)
	LANG=de_AT $(BIN)/src2doc $< | $(BIN)/efeudoc -Tman - | gzip > $@
	$(BIN)/src2doc -a $< | $(BIN)/efeu-symlinks -s .smh.7.gz $@

$(MAN)/de/man7/%.7.gz: $(LIB)/mksource/%.ms
	@mkdir -p $(@D)
	LANG=de_AT $(BIN)/src2doc $< | $(BIN)/efeudoc -Tman - | gzip > $@
	$(BIN)/src2doc -a $< | $(BIN)/efeu-symlinks -s .7.gz $@

# remove dead links

clean::
	find -L $(MAN) -type l -exec rm -vf '{}' \;

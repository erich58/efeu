# :*: libetk - documentation
# :de: libetk - Dokumentation

TOP=../../..
NAME=etk
include $(TOP)/Setup.mak

EFEUDOC=$(BIN)/efeudoc -H '@LIB=../lib/'
DE=$(DOC)/de
EN=$(DOC)/en

FILES = $(DE)/pdf/tk.pdf $(EN)/pdf/tk.pdf
HDIRS = $(DE)/html/tk $(EN)/html/tk
FILES += $(foreach dir,$(HDIRS),$(dir)/index.html)

doc:: files

doc:: $(FILES)

clean::
	rm -rf $(FILES) $(HDIRS)

$(DE)/pdf/tk.pdf:
	@mkdir -p $(@D)
	LANG=de_AT $(EFEUDOC) --pdf -o $@ main.doc

$(DE)/html/tk/index.html:
	@mkdir -p $(@D)
	LANG=de_AT $(EFEUDOC) -h -o $(@D) main.doc

files:: $(BUILD)/de.d

$(BUILD)/de.d:
	@mkdir -p $(@D)
	LANG=de_AT $(EFEUDOC) -d --pdf -o $(DE)/pdf/tk.pdf main.doc > $@
	LANG=de_AT $(EFEUDOC) -d -h -o $(DE)/html/tk main.doc >> $@

include $(BUILD)/de.d

$(EN)/pdf/tk.pdf:
	@mkdir -p $(@D)
	LANG=en_US $(EFEUDOC) --pdf -o $@ main.doc

$(EN)/html/tk/index.html: $(BUILD)/en.d
	@mkdir -p $(@D)
	LANG=en_US $(EFEUDOC) -h -o $(@D) main.doc

files:: $(BUILD)/en.d

$(BUILD)/en.d:
	@mkdir -p $(@D)
	LANG=en_US $(EFEUDOC) -d --pdf -o $(EN)/pdf/tk.pdf main.doc > $@
	LANG=en_US $(EFEUDOC) -d -h -o $(EN)/html/tk main.doc > $@

include $(BUILD)/en.d

clean::
	rm -rf $(BUILD)

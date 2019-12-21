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

$(DE)/pdf/tk.pdf: main.doc
	@mkdir -p $(BUILD) $(@D)
	LANG=de_AT $(EFEUDOC) -M $(BUILD)/de_pdf.d --pdf -o $@ $<

-include $(BUILD)/de_pdf.d

$(DE)/html/tk/index.html: main.doc
	@mkdir -p $(BUILD) $(@D)
	LANG=de_AT $(EFEUDOC) -M $(BUILD)/de_html.d -h -o $(@D) $<

-include $(BUILD)/de_html.d

$(EN)/pdf/tk.pdf: main.doc
	@mkdir -p $(BUILD) $(@D)
	LANG=en_US $(EFEUDOC) -M $(BUILD)/en_pdf.d --pdf -o $@ $<

-include $(BUILD)/en_pdf.d

$(EN)/html/tk/index.html: main.doc
	@mkdir -p $(BUILD) $(@D)
	LANG=en_US $(EFEUDOC) -M $(BUILD)/en_html.d -h -o $(@D) $<

-include $(BUILD)/en_html.d

clean::
	rm -rf $(BUILD)

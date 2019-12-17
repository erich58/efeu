# :*: german documentation about efeudoc
# :de: Handbuch zum Dokumentkonverter

TOP=../../..
include $(TOP)/Setup.mak
include $(CFG)/Config.mak

PDF = $(DOC)/de/pdf/$(NAME).pdf
HTML = $(DOC)/de/html/$(NAME)

doc:: $(PDF) $(HTML)/index.html

clean::
	rm -rf $(PDF) $(HTML) $(BUILD)

$(PDF): main.eds
	@mkdir -p $(BUILD) $(@D)
	LANG=de_AT $(BIN)/efeudoc -M $(BUILD)/pdf.d --pdf -o $@ $<

-include $(BUILD)/pdf.d

$(HTML)/index.html: main.eds
	@mkdir -p $(BUILD) $(@D)
	LANG=de_AT $(BIN)/efeudoc -M $(BUILD)/html.d -h -o $(@D) $<

-include $(BUILD)/html.d

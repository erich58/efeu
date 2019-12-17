# :*: mdmat specification
# :de: MDMAT Spezifikation

TOP=../../..
include $(TOP)/Setup.mak
include $(CFG)/Config.mak

EN_PDF = $(DOC)/en/pdf/$(NAME).pdf
EN_HTML = $(DOC)/en/html/$(NAME)

FILES = $(EN_PDF)
DIRS = $(EN_HTML)
FILES += $(patsubst %,%/index.html,$(DIRS))

doc:: $(FILES)

clean::
	rm -rf $(FILES) $(DIRS) $(BUILD)

$(EN_PDF): main.eds
	@mkdir -p $(BUILD) $(@D)
	LANG=de_AT $(BIN)/efeudoc -M $(BUILD)/en_pdf.d --pdf -o $@ $<

-include $(BUILD)/en_pdf.d

$(EN_HTML)/index.html: main.eds
	@mkdir -p $(BUILD) $(@D)
	LANG=en_US $(BIN)/efeudoc -M $(BUILD)/en_html.d -h -o $(@D) $<

-include $(BUILD)/en_html.d

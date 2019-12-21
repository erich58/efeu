# :*: user reference manual
# :de: Benutzerhandbuch

TOP=../../..
include $(TOP)/Setup.mak
include $(CFG)/Config.mak

DE_PDF = $(DOC)/de/pdf/$(NAME).pdf
EN_PDF = $(DOC)/en/pdf/$(NAME).pdf
DE_HTML = $(DOC)/de/html/$(NAME)
EN_HTML = $(DOC)/en/html/$(NAME)

FILES = $(DE_PDF) $(EN_PDF)
DIRS = $(DE_HTML) $(EN_HTML)
FILES += $(patsubst %,%/index.html,$(DIRS))

doc:: $(FILES)

clean::
	rm -rf $(FILES) $(DIRS) $(BUILD)

$(DE_PDF): main.doc
	@mkdir -p $(BUILD) $(@D)
	LANG=de_AT $(BIN)/efeudoc -M $(BUILD)/de_pdf.d --pdf -o $@ $<

$(EN_PDF): main.doc
	@mkdir -p $(BUILD) $(@D)
	LANG=de_AT $(BIN)/efeudoc -M $(BUILD)/en_pdf.d --pdf -o $@ $<

-include $(BUILD)/de_pdf.d
-include $(BUILD)/en_pdf.d

$(DE_HTML)/index.html: main.doc
	@mkdir -p $(BUILD) $(@D)
	LANG=en_US $(BIN)/efeudoc -M $(BUILD)/de_html.d -h -o $(@D) $<

$(EN_HTML)/index.html: main.doc
	@mkdir -p $(BUILD) $(@D)
	LANG=en_US $(BIN)/efeudoc -M $(BUILD)/en_html.d -h -o $(@D) $<

-include $(BUILD)/de_html.d
-include $(BUILD)/en_html.d

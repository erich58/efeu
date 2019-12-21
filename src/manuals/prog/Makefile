# :*: german documentation about EFEU programming
# :de: EFEU Programmierhandbuch

TOP=../../..
include $(TOP)/Setup.mak
include $(CFG)/Config.mak

DE_PDF = $(DOC)/de/pdf/$(NAME).pdf
DE_HTML = $(DOC)/de/html/$(NAME)

FILES = $(DE_PDF)
DIRS = $(DE_HTML)
FILES += $(patsubst %,%/index.html,$(DIRS))

doc:: $(FILES)

clean::
	rm -rf $(FILES) $(DIRS) $(BUILD)

$(DE_PDF): main.doc
	@mkdir -p $(BUILD) $(@D)
	LANG=de_AT $(BIN)/efeudoc -M $(BUILD)/de_pdf.d --pdf -o $@ $<

-include $(BUILD)/de_pdf.d

$(DE_HTML)/index.html: main.doc
	@mkdir -p $(BUILD) $(@D)
	LANG=de_AT $(BIN)/efeudoc -M $(BUILD)/de_html.d -h -o $(@D) $<

-include $(BUILD)/de_html.d

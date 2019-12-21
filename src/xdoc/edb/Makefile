# :*: The EDB interface
# :de: Die EDB-Datenschnittstelle

TOP=../../..
include $(TOP)/Setup.mak
include $(CFG)/Config.mak

# ACHTUNG: efeudoc muss im Verzeichnis examples aufgerufen werden.

CWD = $(shell pwd)
MAIN = $(CWD)/main.eds 

DE_PDF = $(DOC)/de/pdf/$(NAME).pdf
EN_PDF = $(DOC)/en/pdf/$(NAME).pdf
DE_HTML = $(DOC)/de/html/$(NAME)
EN_HTML = $(DOC)/en/html/$(NAME)

FILES = $(DE_PDF) $(EN_PDF)
DIRS = $(DE_HTML) $(EN_HTML)
FILES += $(patsubst %,%/index.html,$(DIRS))


doc:: $(FILES)

clean::
	rm -rf $(FILES) $(DIRS)

$(DE_PDF): $(MAIN)
	@mkdir -p $(@D)
	cd examples && LANG=de_AT $(BIN)/efeudoc --pdf -o $@ $<

$(EN_PDF): $(MAIN)
	@mkdir -p $(@D)
	cd examples && LANG=de_AT $(BIN)/efeudoc --pdf -o $@ $<

$(DE_HTML)/index.html: $(MAIN)
	@mkdir -p $(@D)
	cd examples && LANG=en_US $(BIN)/efeudoc -h -o $(@D) $<

$(EN_HTML)/index.html: $(MAIN)
	@mkdir -p $(@D)
	cd examples && LANG=en_US $(BIN)/efeudoc -h -o $(@D) $<

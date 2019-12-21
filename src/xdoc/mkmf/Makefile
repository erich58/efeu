# :*: Makefile generation
# :de: Generieren von Makefiles

TOP=../../..
include $(TOP)/Setup.mak
include $(CFG)/Config.mak

# ACHTUNG: efeudoc muss im Verzeichnis example aufgerufen werden.

CWD = $(shell pwd)
MAIN = $(CWD)/main.eds 

DE_PDF = $(DOC)/de/pdf/$(NAME).pdf
DE_HTML = $(DOC)/de/html/$(NAME)

FILES = $(DE_PDF)
DIRS = $(DE_HTML)
FILES += $(patsubst %,%/index.html,$(DIRS))

doc:: $(FILES)

clean::
	rm -rf $(FILES) $(DIRS)

$(DE_PDF): $(MAIN)
	@mkdir -p $(@D)
	cd example && LANG=de_AT $(BIN)/efeudoc --pdf -o $@ $<

$(DE_HTML)/index.html: $(MAIN)
	@mkdir -p $(@D)
	cd example && LANG=en_US $(BIN)/efeudoc -h -o $(@D) $<

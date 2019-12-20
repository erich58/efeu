# :de: Latex-Stylefiles

TOP=../../..
include $(TOP)/Setup.mak

TEXMF = $(TOP)/share/texmf
TEXDIR = $(TEXMF)/tex/latex/misc

VPATH = $(shell find -L . -type d)
STYLE = $(shell find -L . -name '*.sty')
FILES = $(patsubst %,$(TEXDIR)/%,$(notdir $(STYLE)))

all:: $(FILES)

clean::
	rm -f $(FILES)

$(TEXDIR)/% : %
	@mkdir -p $(@D)
	cp $< $@

all::
	-texhash $(TEXMF)

# :de: vim-Konfigurationsdateien

TOP=../../..
include $(TOP)/Setup.mak

VIM = $(TOP)/share/vim

VIMRC = $(wildcard *.vimrc)
TXT = $(wildcard *.txt)
FILES = $(patsubst %,$(VIM)/%,$(VIMRC) $(TXT))

all:: $(FILES)

clean::
	rm -f $(FILES)

$(VIM)/%.vimrc : %.vimrc
	@mkdir -p $(@D)
	sed -e 's|VIM|$(VIM)|g' $< > $@

$(VIM)/%.txt : %.txt
	@mkdir -p $(@D)
	cp $< $@


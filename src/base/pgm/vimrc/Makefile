# :*: vim configuration files
# :de: vim Konfigurationsdateien

TOP=../../../..
include $(TOP)/Setup.mak
include $(CFG)/EFEU.mak

VIMDIR=$(SHR)/vim

FILES  = $(patsubst %.sh,$(BIN)/%,$(wildcard *.sh))
FILES += $(patsubst %,$(VIMDIR)/%,$(wildcard *.vimrc))
FILES += $(patsubst %,$(VIMDIR)/%,$(wildcard *.vim */*.vim))

all:: $(FILES)

clean::
	rm -f $(FILES)

$(BIN)/% : %.sh
	$(BIN)/efeuscript -s "|:VN:|$(VERSION)|" $< $@

$(VIMDIR)/%.vimrc : %.vimrc
	@mkdir -p $(@D)
	sed -e 's|VIMDIR|$(VIMDIR)|g' $< > $@

$(VIMDIR)/%.vim : %.vim
	@mkdir -p $(@D)
	cp $< $@


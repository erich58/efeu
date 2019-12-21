# :*: shell scripts
# :de: Shell-Scripts

TOP=../../..
include $(TOP)/Setup.mak

SCRIPTS = $(wildcard *.sh)
FILES = $(patsubst %.sh,$(BIN)/%,$(SCRIPTS))
LINKS = $(BIN)/tex2pdf

all:: $(FILES) $(LINKS)

clean::
	rm -f $(FILES) $(LINKS)

$(BIN)/% : %.sh
	mkdir -p $(@D)
	sed -e 's/:VN:/$(VERSION)/g' $< > $@
	chmod a+x $@

$(BIN)/tex2pdf: $(BIN)/tex2ps
	cd $(BIN) && ln -fs $(<F) $(@F)

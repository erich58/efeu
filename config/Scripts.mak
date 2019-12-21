# Install Scripts
# :de: Skripts installieren

ifndef _Scripts_mak
_Scripts_mak = defined

_Scripts_src = $(wildcard *.sh)
_Scripts_tg = $(patsubst %.sh,$(BIN)/%,$(_Scripts_src))

all:: $(_Scripts_tg)

clean::
	rm -f $(_Scripts)

$(BIN)/% : %.sh
	mkdir -p $(@D)
	sed -e 's/:VN:/$(VERSION)/g' $< > $@
	chmod a+x $@

endif

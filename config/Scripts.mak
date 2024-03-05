# Install Scripts
# :de: Skripts installieren

ifndef _Scripts_mak
_Scripts_mak = defined

_Scripts_sh = $(shell find -L . -name '*.sh' \
	! -name '*.tst.sh' ! -name '*.ext.sh' -printf '%f\n')
_Scripts_esh = $(shell find -L . -name '*.esh' -printf '%f\n')
_Scripts_etk = $(shell find -L . -name '*.etk' -printf '%f\n')
_Scripts = $(patsubst %.sh,$(BIN)/%,$(_Scripts_sh))
_Scripts += $(patsubst %.esh,$(BIN)/%,$(_Scripts_esh))
_Scripts += $(patsubst %.etk,$(BIN)/%,$(_Scripts_etk))

all:: $(_Scripts)

clean::
	rm -f $(_Scripts)

$(BIN)/% : %.sh
	mkdir -p $(@D)
	sed -e 's/:VN:/$(VERSION)/g' $< > $@
	chmod a+x $@

$(BIN)/% : %.esh
	mkdir -p $(@D)
	$(BIN)/efeuscript -s '/:VN:/$(VERSION)/g' -e -c esh $< $@

$(BIN)/% : %.etk
	mkdir -p $(@D)
	$(BIN)/efeuscript -s '/:VN:/$(VERSION)/g' -e -c etk $< $@

endif

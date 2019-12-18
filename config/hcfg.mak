# update configuration header

ifndef _hcfg_mak_
_hcfg_mak_ = defined

include $(CFG)/EFEU.mak

_hcfg_src = $(wildcard *.hcfg */*.hcfg)
_hcfg_files = $(patsubst %.hcfg,$(EFEU_INCLUDE)/%.h,$(notdir $(_hcfg_src)))

# Die Aktualisierung erfolgt über Pseudo-Targets mit Basisnamen
# der Konfigurationmsdatei.

_hcfg_update = $(patsubst %.hcfg,%,$(_hcfg_src))

.PHONY: $(_hcfg_update)

files:: $(_hcfg_update)

$(_hcfg_update)::
	$(BIN)/efeu-config update $@.hcfg $(EFEU_INCLUDE)/$(@F).h

clean::
	rm -f $(_hcfg_files)

endif

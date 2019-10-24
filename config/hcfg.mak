# update configuration header

ifndef _hcfg_mak_
_hcfg_mak_ = defined

include $(CFG)/EFEU.mak

_hcfg_src = $(wildcard *.hcfg */*.hcfg)
_hcfg_rules = $(patsubst %.hcfg,$(BUILD)/%.hrule,$(notdir $(_hcfg_src)))
_hcfg_files = $(patsubst %.hcfg,$(EFEU_INCLUDE)/%.h,$(notdir $(_hcfg_src)))

files:: $(_hcfg_files)

clean::
	rm -f $(_hcfg_files)

$(_hcfg_rules): $(CFG)/hcfg.mak

include $(_hcfg_rules)

$(BUILD)/%.hrule: %.hcfg
	@mkdir -p $(@D)
	printf '%s::\n\t@$(BIN)/efeu-config update %s $$@\n' \
		$(EFEU_INCLUDE)/$(@F:.hrule=.h) $< > $@

endif

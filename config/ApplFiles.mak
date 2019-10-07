# Application Files

ifndef _ApplFiles_mak_
_ApplFiles_mak_ = defined

include $(CFG)/EFEU.mak

_ApplFiles_update = sh $(CFG)/efeu-update.sh -v
_ApplFiles_cfgrule = sh $(CFG)/efeu-cfgrule.sh -a files -l de

-include $(BUILD)/cnf.rule
-include $(BUILD)/hlp.rule
-include $(BUILD)/msg.rule

$(BUILD)/cnf.files::
	@$(_ApplFiles_update) $@ find . -name '*.cnf'

$(BUILD)/cnf.rule: $(BUILD)/cnf.files
	@$(_ApplFiles_cfgrule) $< $(LIB)/efeu/%L/config/%F > $@

$(BUILD)/hlp.files::
	@$(_ApplFiles_update) $@ find . -name '*.hlp'

$(BUILD)/hlp.rule: $(BUILD)/hlp.files
	@$(_ApplFiles_cfgrule) $< $(LIB)/efeu/%L/help/%F > $@

$(BUILD)/msg.files::
	@$(_ApplFiles_update) $@ find . -name '*.msg'

$(BUILD)/msg.rule: $(BUILD)/msg.files
	@$(_ApplFiles_cfgrule) $< $(LIB)/efeu/%L/messages/%F > $@

endif

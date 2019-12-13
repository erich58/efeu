# Application Files

ifndef _ApplFiles_mak_
_ApplFiles_mak_ = defined

include $(CFG)/EFEU.mak

-include $(BUILD)/cnf.rule
-include $(BUILD)/hlp.rule
-include $(BUILD)/msg.rule

$(BUILD)/cnf.files::
	@$(BIN)/efeu-update $@ find . -name '*.cnf'

$(BUILD)/cnf.rule: $(BUILD)/cnf.files $(BIN)/efeu-mkrule
	@$(BIN)/efeu-mkrule -a files -l de $< $(LIB)/efeu/%L/config/%F > $@

$(BUILD)/hlp.files::
	@$(BIN)/efeu-update $@ find . -name '*.hlp'

$(BUILD)/hlp.rule: $(BUILD)/hlp.files $(BIN)/efeu-mkrule
	@$(BIN)/efeu-mkrule -a files -l de $< $(LIB)/efeu/%L/help/%F > $@

$(BUILD)/msg.files::
	@$(BIN)/efeu-update $@ find . -name '*.msg'

$(BUILD)/msg.rule: $(BUILD)/msg.files $(BIN)/efeu-mkrule
	@$(BIN)/efeu-mkrule -a files -l de $< $(LIB)/efeu/%L/messages/%F > $@

clean::
	rm -rf $(BUILD)
endif

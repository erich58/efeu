# Magic Files

ifndef _MagicFiles_mak_
_MagicFiles_mak_ = defined

include $(CFG)/EFEU.mak

ifndef	MAGIC
MAGIC = $(TOP)/etc/efeu
endif

-include $(BUILD)/magic.rule

$(BUILD)/magic.files::
	@$(BIN)/efeu-update $@ find . -name '*.magic'

$(BUILD)/magic.rule: $(BUILD)/magic.files
	$(BIN)/efeu-mkrule -a files $< $(MAGIC)/magic.d/%B > $@

all clean::
	-$(BIN)/efeu-magic $(MAGIC)

endif

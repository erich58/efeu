# Application Files

ifndef _ApplFiles_mak_
_ApplFiles_mak_ = defined

include $(CFG)/Config.mak

all clean::
	$(BIN)/efeu-applfiles $(EFEU_LIB) $@

endif

# EFEU specific extensions

ifndef _EFEU_mak_
_EFEU_mak_ = defined

include $(CFG)/Config.mak
include $(CFG)/CC.mak

ifndef	EFEU_INCLUDE
EFEU_INCLUDE = $(INC)/EFEU
endif

CPPFLAGS += -D'EFEU_VERSION="$(VERSION)"' \
	-D_XOPEN_SOURCE=500 -D_FILE_OFFSET_BITS=64

EFEU_DIRS = $(INC) $(BIN) $(LIB) 

files:: $(EFEU_DIRS)

$(EFEU_DIRS)::
	@mkdir -p $@

# build directory

files:: $(BUILD)

clean::
	rm -rf $(BUILD)

$(BUILD)::
	@mkdir -p $@

endif

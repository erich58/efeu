# EFEU specific extensions

ifndef _EFEU_mak_
_EFEU_mak_ = defined

include $(CFG)/Config.mak
include $(CFG)/CC.mak

ifndef	EFEU_INCLUDE
EFEU_INCLUDE = $(INC)/EFEU
endif

ifndef	EFEU_LIB
EFEU_LIB = $(LIB)/efeu
endif

ifndef	VPATH
VPATH = $(shell find -L . -type d)
endif

CPPFLAGS += -D'EFEU_VERSION="$(VERSION)"' \
	-D_XOPEN_SOURCE=600 -D_FILE_OFFSET_BITS=64

ifndef	TST_CFLAGS
TST_CFLAGS = -D'TOP="$(_TOP)"'
endif

EFEU_DIRS = $(INC) $(BIN) $(LIB) 

files:: $(EFEU_DIRS)

$(EFEU_DIRS)::
	@mkdir -p $@

endif

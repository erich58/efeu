# EFEU specific extensions

ifndef _EFEU_mak_
_EFEU_mak_ = defined

ifeq ($(wildcard $(TOP)/CHANGELOG),)
$(error "$$(TOP)/CHANGELOG not found")
endif

include $(CFG)/CC.mak

EFEU_VERSION=$(shell sed -e 1q $(TOP)/CHANGELOG)
CPPFLAGS += -D'EFEU_VERSION="$$EFEU_VERSION"' -D_XOPEN_SOURCE=500

EFEU_DIRS = $(INC) $(BIN) $(LIB) 

all:: files

files:: $(EFEU_DIRS)

clean::

$(EFEU_DIRS)::
	@mkdir -p $@

# build directory

files:: $(BUILD)

clean::
	rm -rf $(BUILD)

$(BUILD)::
	mkdir -p $@

endif

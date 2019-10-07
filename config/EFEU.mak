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

# local source files

VPATH = $(shell find -L . -type d)
HCFG = $(wildcard *.hcfg */*.hcfg)
HDR = $(wildcard *.h */*.h)
SRC = $(wildcard *.c */*.c)

OBJ = $(patsubst %.c,$(BUILD)/%.o,$(notdir $(SRC)))

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

# header files

ifndef	EFEU_INCLUDE
EFEU_INCLUDE = $(INC)/EFEU
endif

FILES = $(patsubst %.hcfg,$(EFEU_INCLUDE)/%.h,$(notdir $(HCFG)))
FILES += $(patsubst %.h,$(EFEU_INCLUDE)/%.h,$(notdir $(HDR)))

files:: $(FILES)

clean::
	rm -f $(FILES)

# update configuration header

HRULES = $(patsubst %.hcfg,$(BUILD)/%.hrule,$(notdir $(HCFG)))

include $(HRULES)

CONFIG_UPDATE = sh $(CFG)/efeu-config.sh update

$(BUILD)/%.hrule: %.hcfg
	@mkdir -p $(@D)
	printf '%s::\n\t$(CONFIG_UPDATE) %s $$@\n' \
		$(EFEU_INCLUDE)/$(@F:.hrule=.h) $< > $@

# installing header files

$(EFEU_INCLUDE)/%.h : %.h
	@mkdir -p $(@D)
	cp $< $@

# build object files

$(BUILD)/%.d : %.o
	@mkdir -p $(@D)
	echo '$(WORK)/cp $<' > $@

$(BUILD)/%.o : %.c
	@mkdir -p $(@D)
	$(CC) -o $@ -c -MMD $(CPPFLAGS) $(CFLAGS) $<

endif

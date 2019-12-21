TOP=../..
include $(TOP)/Setup.mak
include $(CFG)/Config.mak

CTAGS = $(shell sh ./ctags.sh)

ifeq ($(CTAGS),)
all:: $(TOP)/note/ctags

$(TOP)/note/ctags:
	@mkdir -p $(@D)
	@printf '%s\n' \
	'The command ctags is not available, so no tag file is created.' \
	'This is not needed for running efeu-commands, but tag commands' \
	'could not be used to jump to the definition of a specific symbol' \
	'on visiting the sources.' | tee $@
else

all::
	@rm -vf $(TOP)/note/ctags

all:: $(TOP)/tags

$(TOP)/tags: $(BUILD)/tags
	cp $< $@

$(BUILD)/tags: $(BUILD)/C $(BUILD)/H
	cd $(BUILD); rm -f tags; cat C H | xargs $(CTAGS)

$(BUILD)/C::
	@mkdir -p $(@D)
	$(BIN)/efeu-update $@ find $(SRC) -name '*.c'

$(BUILD)/H::
	@mkdir -p $(@D)
	$(BIN)/efeu-update $@ find $(SRC) -name '*.h'

clean::
	rm -f $(TOP)/tags
	rm -rf $(BUILD)
endif

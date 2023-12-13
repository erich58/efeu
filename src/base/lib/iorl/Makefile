# :*: readline interface
# :de: Readline-Schnittstelle

TOP=../../../..
include $(TOP)/Setup.mak
include $(CFG)/EFEU.mak

prereq::
	make -C ../efm

RL_CONFIG= $(EFEU_INCLUDE)/rl_config.h

LDFLAGS += -lefm -lreadline -lncurses -ldl
TST_LDFLAGS += -lutil
TST_CFLAGS += -I ../efm

files:: $(RL_CONFIG)

clean::
	rm -f $(RL_CONFIG)

$(RL_CONFIG)::
	@mkdir -p $(@D)
	$(BIN)/efeu-config -t $(TOP) update rl_config.hcfg $@
	@mkdir -p $(TOP)/note
	sh ./verify.sh $(TOP)/note/iorl $@

include $(CFG)/Library.mak
include $(CFG)/ApplFiles.mak

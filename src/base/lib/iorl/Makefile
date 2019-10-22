include Setup.mak
include $(CFG)/EFEU.mak

RL_CONFIG= $(EFEU_INCLUDE)/rl_config.h

include Rules.mak

files:: $(RL_CONFIG)

clean::
	rm -f $(RL_CONFIG)

$(RL_CONFIG)::
	@mkdir -p $(@D)
	/home/erich/efeu/bin/efeu-config -t $(TOP) update rl_config.hcfg $@
	@mkdir -p $(TOP)/note
	sh ./verify.sh $(TOP)/note/iorl $@

include $(CFG)/Library.mak
include $(CFG)/ApplFiles.mak

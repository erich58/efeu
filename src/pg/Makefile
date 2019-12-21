# :*: PostgreSQL-interface
# :de: Schnittstelle zu PostgreSQL

include config.mk

prereq::
	make -C $(TOP)/src/base

# Header für PostgreSQL Schnittstelle

PQ_CONFIG= $(EFEU_INCLUDE)/pq_config.h

files:: $(PQ_CONFIG)

$(PQ_CONFIG)::
	@mkdir -p $(@D)
	$(BIN)/efeu-config -t $(TOP) update pq_config.hcfg $@
	@mkdir -p $(TOP)/note
	sh ./verify.sh $(TOP)/note/pq $@

# Stufe 1: Sourcen bereitstellen

TPL = $(wildcard *.tpl)
SUBTARGET = $(patsubst %.tpl,%,$(TPL))
SUBMAKE = $(patsubst %,$(BUILD)/%.mk,$(SUBTARGET))

files:: $(SUBMAKE)

all:: $(SUBTARGET)

% : $(BUILD)/%.mk
	@mkdir -p $(@D)
	$(MAKE) -f $<

$(BUILD)/%.mk : %.tpl
	@mkdir -p $(@D)
	$(BIN)/mksource -W $(BUILD) -dr $< > $@

# Stufe 2: Header und Binaries

all clean::
	$(MAKE) -f post.mk $@

clean::
	rm -f $(PQ_CONFIG)

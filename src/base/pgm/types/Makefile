# :*: Supplemented data types for interpreter
# :de: Ergänzende Datentypen für Interpreter

TOP=../../../..
include $(TOP)/Setup.mak
include $(CFG)/EFEU.mak

prereq::
	$(MAKE) -C ../esh

# Stufe 1: Sourcen bereitstellen

VPATH = $(shell find -L . -type d)
TPL = $(wildcard */*.tpl)
SUBTARGET = $(patsubst %.tpl,%,$(notdir $(TPL)))
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

# :*: EFEU Letters and Numbers
# :de: EFEU-Version von Buchstaben und Zahlen

TOP=../../../..
include $(TOP)/Setup.mak
include $(CFG)/Config.mak

prereq::
	make -C $(TOP)/src/tk

PGM = $(BIN)/efeu-letters

all:: $(PGM)

clean::
	rm -f $(PGM)

$(PGM): efeu-letters.etk
	@mkdir -p $(@D)
	$(TOP)/bin/efeuscript -e -c etk $< $@

# Dateien

TG_DIR=$(SHR)/eletters
IM_SRC = $(wildcard images/*)
IM_TG = $(patsubst %,$(TG_DIR)/%,$(IM_SRC))

files:: $(IM_TG)

clean::
	rm -rf $(IM_TG)

$(TG_DIR)/images/% : images/%
	@mkdir -p $(@D)
	cp $< $@

# Links

all clean:: $(BUILD)/en.mak
	$(MAKE) -f $(BUILD)/en.mak $@

all clean:: $(BUILD)/de.mak
	$(MAKE) -f $(BUILD)/de.mak $@

$(BUILD)/en.mak::
	@mkdir -p $(@D)
	$(BIN)/efeu-update $@ sh mkrule.sh $(TG_DIR) en.rules

$(BUILD)/de.mak::
	@mkdir -p $(@D)
	$(BIN)/efeu-update $@ sh mkrule.sh $(TG_DIR) de.rules

clean::
	rm -rf $(BUILD)

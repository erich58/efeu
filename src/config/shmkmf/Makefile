# :*: create Makefile from shell
# :de: Makefile mithilfe der Shell generieren

TOP=../../..
include $(TOP)/Setup.mak

TGLIB = $(LIB)/shmkmf

SCRIPTS = $(wildcard *.sh)
SRC = $(wildcard *.smh *.msg *.awk)
EN_HLP = $(wildcard en/*.hlp)
DE_HLP = $(wildcard de/*.hlp)

FILES = $(patsubst %.sh,$(BIN)/%,$(SCRIPTS))
FILES += $(patsubst %,$(TGLIB)/%,$(SRC))
FILES += $(patsubst %,$(LIB)/efeu/help/%,$(notdir $(EN_HLP)))
FILES += $(patsubst %,$(LIB)/efeu/de/help/%,$(notdir $(DE_HLP)))

all:: $(FILES)

clean::
	rm -f $(FILES)

$(BIN)/% : %.sh
	mkdir -p $(@D)
	sed -e 's/:VN:/$(VERSION)/g' $< > $@
	chmod a+x $@

$(TGLIB)/% : %
	@mkdir -p $(@D)
	cp $< $@
	
$(LIB)/efeu/help/% : en/%
	@mkdir -p $(@D)
	cp $< $@
	
$(LIB)/efeu/de/help/% : de/%
	@mkdir -p $(@D)
	cp $< $@

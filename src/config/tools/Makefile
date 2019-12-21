# :*: create Makefile from shell
# :de: Makefile mithilfe der Shell generieren

TOP=../../..
include $(TOP)/Setup.mak
include $(CFG)/SimplePrograms.mak

SH = $(wildcard *.sh)
EN_HLP = $(wildcard *.hlp)
DE_HLP = $(wildcard de/*.hlp)

FILES = $(patsubst %.sh,$(BIN)/%,$(SH))
FILES += $(patsubst %,$(LIB)/efeu/help/%,$(EN_HLP))
FILES += $(patsubst de/%,$(LIB)/efeu/de/help/%,$(DE_HLP))

all:: $(FILES)

clean::
	rm -f $(FILES)

$(BIN)/% : %.sh
	mkdir -p $(@D)
	sed -e 's/:VN:/$(VERSION)/g' $< > $@
	chmod a+x $@

$(LIB)/efeu/help/% : %
	@mkdir -p $(@D)
	cp $< $@
	
$(LIB)/efeu/de/help/% : de/%
	@mkdir -p $(@D)
	cp $< $@

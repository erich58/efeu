# :*: makefile generator based on cpp
# :de: Makefile Generator, basierend auf cpp

TOP=../../../..
include $(TOP)/Setup.mak
include $(CFG)/CC.mak

PPINCLUDE = $(TOP)/ppinclude

VPATH = $(shell find -L . -type d)
pph_files = $(wildcard */*.pph)
cfg_files = $(wildcard */*.cfg)
sh_files = $(wildcard *.sh */*.sh)

FILES = $(patsubst %,$(PPINCLUDE)/%,$(notdir $(pph_files)))
FILES += $(patsubst %,$(PPINCLUDE)/%,$(notdir $(cfg_files)))
FILES += $(patsubst %.sh,$(BIN)/%,$(notdir $(sh_files)))

FILES += $(BIN)/ppfilter
FILES += $(LIB)/efeu/config/ppfilter.cnf
FILES += $(LIB)/efeu/help/ppfilter.hlp
FILES += $(LIB)/efeu/de/help/ppfilter.hlp

all:: $(FILES)

clean::
	rm -f $(FILES)

$(PPINCLUDE)/%.pph : %.pph
	@mkdir -p $(@D)
	cp $< $@

$(PPINCLUDE)/%.cfg : %.cfg
	@mkdir -p $(@D)
	sed -e 's/:VN:/$(VERSION)/g' $< > $@

$(BIN)/% : %.sh
	@mkdir -p $(@D)
	sed -e 's/:VN:/$(VERSION)/g' $< > $@
	chmod a+x $@

# PPFILTER

$(BIN)/ppfilter: ppfilter.c
	@mkdir -p $(@D)
	$(CC) -o $@ $(CPPFLAGS) $(CFLAGS) $< $(LDFLAGS)

$(LIB)/efeu/config/ppfilter.cnf: ppfilter.cnf
	@mkdir -p $(@D)
	sed -e 's/:VN:/$(VERSION)/g' $< > $@

$(LIB)/efeu/help/ppfilter.hlp: ppfilter.hlp
	@mkdir -p $(@D)
	cp $< $@

$(LIB)/efeu/de/help/ppfilter.hlp: de/ppfilter.hlp
	@mkdir -p $(@D)
	cp $< $@

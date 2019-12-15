# :*: EFEU specific tools
# :de: EFEU spezifische Werkzeuge

TOP=../../..
include $(TOP)/Setup.mak

TGLIB = $(LIB)/shmkmf

SCRIPTS = $(wildcard *.sh)
GENCMD = $(wildcard *.gencmd)
SRC = $(wildcard *.smh)

FILES = $(patsubst %.sh,$(BIN)/%,$(SCRIPTS))
FILES += $(patsubst %.gencmd,$(BIN)/%,$(GENCMD))
FILES += $(patsubst %,$(TGLIB)/%,$(SRC))

all:: $(FILES)

clean::
	rm -f $(FILES)

$(BIN)/% : %.sh
	mkdir -p $(@D)
	sed -e 's/:VN:/$(VERSION)/g' $< > $@
	chmod a+x $@

$(BIN)/% : %.gencmd
	mkdir -p $(@D)
	VN="$(VERSION)" sh $< $(TOP) > $@
	chmod a+x $@

$(TGLIB)/% : %
	@mkdir -p $(@D)
	cp $< $@

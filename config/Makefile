include Setup.mak
include $(CFG)/EFEU.mak
include $(CFG)/CC.mak

SCRIPTS = $(wildcard *.sh)
FILES = $(patsubst %.sh,$(BIN)/%,$(SCRIPTS))

all:: $(FILES)

clean::
	rm -f $(FILES)

$(BIN)/% : %.sh
	mkdir -p $(@D)
	sed -e 's/:VN:/$(EFEU_VERSION)/g' $< > $@
	chmod a+x $@

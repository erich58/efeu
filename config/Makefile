include Setup.mak

SCRIPTS = $(wildcard *.sh)
FILES = $(patsubst %.sh,$(BIN)/%,$(SCRIPTS))

all:: $(FILES)

clean::
	rm -f $(FILES)

$(BIN)/% : %.sh
	mkdir -p $(@D)
	sed -e 's/:VN:/$(VERSION)/g' $< > $@
	chmod a+x $@

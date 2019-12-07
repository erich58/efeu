TOP=../../..
include $(TOP)/Setup.mak
include $(CFG)/EFEU.mak

prereq::
	$(MAKE) -C ../lib

EXDIR = $(SHR)/examples/etk

ETK = $(wildcard *.etk)
FILES = $(patsubst %.etk,$(EXDIR)/%,$(ETK))

all:: $(FILES)

clean::
	rm -f $(FILES)

$(EXDIR)/% : %.etk
	@mkdir -p $(@D)
	$(BIN)/efeuscript -e -c etk $< $@

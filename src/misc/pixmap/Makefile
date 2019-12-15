# :*: PostgreSQL-interface
# :de: Schnittstelle zu PostgreSQL

TOP=../../..
include $(TOP)/Setup.mak
include $(CFG)/EFEU.mak

prereq::
	$(MAKE) -C $(TOP)/src/base

LDFLAGS += -lpix -liorl -lmd -lefmath -lefx -lefm -lm -lX11
include $(CFG)/SimplePrograms.mak
include $(CFG)/ApplFiles.mak

SCRIPTS = $(patsubst %.pix,$(BIN)/%,$(wildcard *.pix))
FLAGS = -s '/:VN:/$(VERSION)/'

all:: $(SCRIPTS)

clean::
	rm -f $(SCRIPTS)

$(BIN)/% : %.pix
	$(BIN)/efeuscript $(FLAGS) -e -c pixmap $< $@

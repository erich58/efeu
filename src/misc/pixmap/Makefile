# :*: PostgreSQL-interface
# :de: Schnittstelle zu PostgreSQL

TOP=../../..
include $(TOP)/Setup.mak
include $(CFG)/EFEU.mak

prereq::
	make -C $(TOP)/src/base

X11 = $(shell $(BIN)/efeu-config cflags -L/usr/X11R6/lib:/usr/local/lib -lX11)

LDFLAGS += -lpix -liorl -lmd -lefmath -lefx -lefm -lm $(X11)
include $(CFG)/SimplePrograms.mak
include $(CFG)/ApplFiles.mak

SCRIPTS = $(patsubst %.pix,$(BIN)/%,$(wildcard *.pix))
FLAGS = -s '/:VN:/$(VERSION)/'

all:: $(SCRIPTS)

clean::
	rm -f $(SCRIPTS)

$(BIN)/% : %.pix
	$(BIN)/efeuscript $(FLAGS) -e -c pixmap $< $@

# :de: etk-interpreter

TOP=../../..
include $(TOP)/Setup.mak
include $(CFG)/EFEU.mak
include $(CFG)/ApplFiles.mak

prereq::
	make -C ../lib

../.build/tklib.mak::
	$(MAKE) -C ../config

include ../.build/tklib.mak

LDFLAGS += -lETK -liorl -lmd -lefmath -lefx -lefm -ldl -lm

SRC = $(wildcard *.c)
PGM = $(patsubst %.c,$(BIN)/%,$(SRC))

all:: $(PGM)

clean::
	rm -f $(PGM)

$(BIN)/% : %.c
	@mkdir -p $(@D)
	$(CC) -o $@ $(CPPFLAGS) $(CFLAGS) $< $(LDFLAGS)

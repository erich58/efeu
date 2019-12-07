TOP=../../..
include $(TOP)/Setup.mak
include $(CFG)/EFEU.mak
include $(CFG)/ApplFiles.mak

prereq::
	$(MAKE) -C ../lib

../.build/tklib.mak:: prereq

include ../.build/tklib.mak

LDFLAGS += -letk -liorl -lmd -lefmath -lefx -lefm -ldl -lm

SRC = $(wildcard *.c)
PGM = $(patsubst %.c,$(BIN)/%,$(SRC))

all:: $(PGM)

clean::
	rm -f $(PGM)

$(BIN)/% : %.c
	@mkdir -p $(@D)
	$(CC) -o $@ $(CPPFLAGS) $(CFLAGS) $< $(LDFLAGS)

# :*: edb commands
# :de: edb  Kommandos

TOP=../../../..
include $(TOP)/Setup.mak
include $(CFG)/EFEU.mak
include $(CFG)/ApplFiles.mak

prereq::
	make -C ../../scripts
	make -C ../../lib

LDFLAGS += -lefmath -liorl -lmd -lefx -lefm -ldl -lm

VPATH = $(shell find -L . -type d)
SRC = $(wildcard */*.c)
PGM = $(patsubst %.c,$(BIN)/%,$(notdir $(SRC)))

all:: $(PGM)

clean::
	rm -f $(PGM)

$(BIN)/% : %.c
	@mkdir -p $(@D)
	$(CC) -o $@ $(CPPFLAGS) $(CFLAGS) $< $(LDFLAGS)


# Scripts

SRC_ESH = $(wildcard script/*.esh)
FILES = $(patsubst %.esh,$(BIN)/%,$(notdir $(SRC_ESH)))

all:: $(FILES)

$(BIN)/% : script/%.esh
	$(BIN)/efeuscript -e -c esh $< $@

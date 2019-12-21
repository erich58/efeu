# :de: Scripts

TOP=../../../..
include $(TOP)/Setup.mak
include $(CFG)/Config.mak

VPATH = $(shell find -L . -type d)
SRC_SH = $(shell find -L . -name '*.sh')
SRC_ESH = $(shell find -L . -name '*.esh')
FLAGS = -s '/:VN:/$(VERSION)/'

FILES = $(patsubst %.sh,$(BIN)/%,$(notdir $(SRC_SH)))
FILES += $(patsubst %.esh,$(BIN)/%,$(notdir $(SRC_ESH)))

all:: $(FILES)

$(BIN)/% : %.sh
	$(BIN)/efeuscript $(FLAGS) $< $@

$(BIN)/% : %.esh
	$(BIN)/efeuscript $(FLAGS) -e -c esh $< $@

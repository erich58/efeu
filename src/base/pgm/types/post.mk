TOP=../../../..
include $(TOP)/Setup.mak
include $(CFG)/EFEU.mak

TINC = $(INC)/EfiTypes
TLIB = $(LIB)/esh/types

VPATH = $(shell find -L . -type d)
HDR = $(shell find -L . -name '*.h')
SRC = $(shell find -L . -name '*.c')

OBJ = $(patsubst %.c,$(BUILD)/%.so,$(notdir $(SRC)))
DEP = $(patsubst %.c,$(BUILD)/%.d,$(notdir $(SRC)))
FILES = $(patsubst %.h,$(TINC)/%.h,$(notdir $(HDR)))
BINARY = $(patsubst %,$(TLIB)/%,$(notdir $(OBJ)))

all:: $(BINARY)

files:: $(FILES)

clean::
	rm -f $(FILES) $(BINARY)

-include $(DEP)

$(BUILD)/%.d : %.c
	echo "$(@:.d=.so): $<" > $@

$(BUILD)/%.so:  %.c
	$(CC) -o $@ -MMD $(CPPFLAGS) $(CFLAGS) -I. -I$(TINC) \
		-shared -nostdlib $< $(LDFLAGS)

$(TINC)/%.h : %.h
	@mkdir -p $(@D)
	cp $< $@

$(TLIB)/% : $(BUILD)/%
	@mkdir -p $(@D)
	cp -a $< $@

TOP=../../../..
include $(TOP)/Setup.mak
include $(CFG)/EFEU.mak

TINC = $(INC)/EfiType
TLIB = $(LIB)/esh/types

VPATH = $(shell find -L . -type d)
HDR = $(shell find -L . -name '*.h' \
	! -name '*.tst.h' ! -name '*.ext.h' -printf '%f\n')
SRC = $(shell find -L . -name '*.c' \
	! -name '*.tst.c' ! -name '*.ext.c' -printf '%f\n')

OBJ = $(patsubst %.c,$(BUILD)/%.so,$(notdir $(SRC)))
DEP = $(patsubst %.c,$(BUILD)/%.d,$(notdir $(SRC)))
FILES = $(patsubst %.h,$(TINC)/%.h,$(notdir $(HDR)))
BINARY = $(patsubst %,$(TLIB)/%,$(notdir $(OBJ)))

all:: $(BINARY)

files:: $(FILES)

clean::
	rm -f $(FILES) $(BINARY)

-include $(DEP)

$(BUILD)/%.so:  %.c
	@mkdir -p $(BUILD)
	$(CC) -o $@ -MMD $(CPPFLAGS) $(CFLAGS) -I. -I$(TINC) \
		-shared -nostdlib $< $(LDFLAGS)

$(TINC)/%.h : %.h
	@mkdir -p $(@D)
	cp $< $@

$(TLIB)/% : $(BUILD)/%
	@mkdir -p $(@D)
	cp -a $< $@

# unit test

GCOV_OBJ = $(patsubst %.c,$(BUILD)/%.gcov.o,$(notdir $(SRC)))

tst_src = $(shell find -L . -name '*.tst.c' -printf '%f\n')
tst_obj = $(patsubst %.c,$(BUILD)/%.o,$(tst_src))
tst_dep = $(patsubst %.o,%.d,$(tst_obj))
tst_bin = $(patsubst %.c,$(BUILD)/%,$(tst_src))

ifneq ($(tst_bin),)

test:: all

test::  $(tst_bin)

$(tst_bin): $(GCOV_OBJ)

test::
	$(BIN)/efeu-test .

TST_CFLAGS += -I $(TOP)/src/base/lib/efm
TST_CFLAGS += -I $(TOP)/src/base/lib/efx
TST_LDFLAGS += -lefx -lefm -lm -ldl

$(BUILD)/%.gcov.o : %.c
	@mkdir -p $(@D)
	$(CC) -o $@ -c -MMD $(CPPFLAGS) $(CFLAGS) --coverage $<

$(BUILD)/%.tst.o : %.tst.c
	@mkdir -p $(@D)
	$(CC) -o $@ -c -MMD $(CPPFLAGS) -I. $(CFLAGS) $(TST_CFLAGS) --coverage $<

$(BUILD)/%.tst : $(BUILD)/%.tst.o
	@mkdir -p $(@D)
	$(CC) -o $@ $< $(@:.tst=.gcov.o) -lgcov $(LDFLAGS) $(TST_LDFLAGS) -lutest

endif

# Create Simple Programs
# Every source file generates its own program

ifndef _SimplePrograms_mak_
_SimplePrograms_mak_ = defined

include $(CFG)/EFEU.mak
include $(CFG)/ApplFiles.mak

_SimplePrograms_src = $(shell find -L . -name '*.c' \
	! -name '*.tst.c' -printf '%f\n')
_SimplePrograms = $(patsubst %.c,$(BIN)/%,$(_SimplePrograms_src))

all:: $(_SimplePrograms)

clean::
	rm -f $(_SimplePrograms)

$(BIN)/% : %.c
	@mkdir -p $(@D)
	$(CC) -o $@ $(CPPFLAGS) $(CFLAGS) $< $(LDFLAGS)

# unit test with code coverage

_SimplePrograms_tst_src = $(shell find -L . -name '*.tst.c' -printf '%f\n')

ifneq ($(_SimplePrograms_tst_src),)
_SimpleProgram_tst = $(patsubst %.c,$(BUILD)/%,$(_SimplePrograms_src))
_SimpleProgram_tst += $(patsubst %.c,$(BUILD)/%,$(_SimplePrograms_tst_src))

test:: $(_SimpleProgram_tst)

test::
	$(BIN)/efeu-test .

TST_CFLAGS += -D 'BUILD="$(BUILD)"'

$(BUILD)/%.gcov.o : %.c
	@mkdir -p $(@D)
	$(CC) -o $@ -c -MMD $(CPPFLAGS) $(CFLAGS) --coverage $<

$(BUILD)/% : $(BUILD)/%.gcov.o
	@mkdir -p $(@D)
	$(CC) -o $@ $< -lgcov $(LDFLAGS)

$(BUILD)/%.tst.o : %.tst.c
	@mkdir -p $(@D)
	$(CC) -o $@ -c -MMD $(CPPFLAGS) -I. $(CFLAGS) \
		$(TST_CFLAGS) --coverage $<

$(BUILD)/%.tst : $(BUILD)/%.tst.o
	@mkdir -p $(@D)
	$(CC) -o $@ $< -lgcov $(LDFLAGS) $(TST_LDFLAGS) -lutest

clean::
	rm -rf $(BUILD)

endif

endif

# Create Complex Program
# All source files defines a single program

ifndef _ComplexProgram_mak_
_ComplexProgram_mak_ = defined

include $(CFG)/EFEU.mak
include $(CFG)/ApplFiles.mak

_ComplexProgram = $(BIN)/$(NAME)
_ComplexProgram_src = $(shell find -L . -name '*.c' \
	! -name '*.tst.c' -printf '%f\n')

_ComplexProgram_obj = $(patsubst %.c,$(BUILD)/%.o,$(_ComplexProgram_src))
_ComplexProgram_dep = $(patsubst %.o,%.d,$(_ComplexProgram_obj))

all:: $(_ComplexProgram)

clean::
	rm -f $(_ComplexProgram) $(_ComplexProgram_files)
	rm -rf $(BUILD)

$(_ComplexProgram): $(_ComplexProgram_obj)
	@mkdir -p $(@D)
	$(CC) -o $@ $(_ComplexProgram_obj) $(LDFLAGS)

-include $(_ComplexProgram_dep)

$(BUILD)/%.o : %.c
	@mkdir -p $(BUILD) $(@D)
	$(CC) -o $@ -c -MMD $(CPPFLAGS) $(CFLAGS) $<

$(LIB)/efeu/config/%.cnf : %.cnf
	@mkdir -p $(@D)
	cp $< $@

# unit test with code coverage

test::
	make -C $(SRC)/utest all

test:: all

_ComplexProgram_tst_src = $(shell find -L . -name '*.tst.c' -printf '%f\n')
_ComplexProgram_tst_obj = $(patsubst %.c,$(BUILD)/%.o,$(_ComplexProgram_tst_src))
_ComplexProgram_tst_dep = $(patsubst %.o,%.d,$(_ComplexProgram_tst_obj))
_ComplexProgram_tst_bin = $(patsubst %.c,$(BUILD)/%,$(_ComplexProgram_tst_src))

ifneq ($(_ComplexProgram_tst_bin),)

_ComplexProgram_gcov = $(BUILD)/lib$(NAME).gcov.a
_ComplexProgram_gcov_obj = $(patsubst %.o,%.gcov.o,$(_ComplexProgram_obj))
_ComplexProgram_gcov_dep = $(patsubst %.o,%.d,$(_ComplexProgram_gcov_obj))

test:: $(_ComplexProgram_gcov)

test:: $(_ComplexProgram_tst_bin)

test::
	$(BIN)/efeu-test .

$(_ComplexProgram_gcov): $(_ComplexProgram_gcov_obj)
	ar -r $@ $(_ComplexProgram_gcov_obj)

-include $(_ComplexProgram_gcov_dep)
-include $(_ComplexProgram_tst_dep)

$(BUILD)/%.gcov.o : %.c
	@mkdir -p $(@D)
	$(CC) -o $@ -c -MMD $(CPPFLAGS) $(CFLAGS) \
		-D 'main=hidden_main(int, char**); int hidden_main' \
		--coverage $<

$(_ComplexProgram_tst_obj) : $(_ComplexProgram_gcov)

$(BUILD)/%.tst.o : %.tst.c
	@mkdir -p $(@D)
	$(CC) -o $@ -c -MMD $(CPPFLAGS) -I. $(CFLAGS) $(TST_CFLAGS) --coverage $<

$(BUILD)/%.tst : $(BUILD)/%.tst.o
	@mkdir -p $(@D)
	$(CC) -o $@ $< $(_ComplexProgram_gcov) -lgcov $(LDFLAGS) $(TST_LDFLAGS) -lutest

purge::
	rm -rf $(_ComplexProgram_gcov)
	rm -rf $(_ComplexProgram_gcov_obj) $(_ComplexProgram_gcov_dep)
	rm -rf $(_ComplexProgram_gcov_obj:.o=.gcno)
	rm -rf $(_ComplexProgram_gcov_obj:.o=.gcda)
	rm -rf $(_ComplexProgram_tst_bin) $(_ComplexProgram_tst_dep)

endif	# _ComplexProgram_tst_bin
endif	# _ComplexProgram_mak_

# Create Library

ifndef _Library_mak_
_Library_mak_ = defined

include $(CFG)/EFEU.mak
include $(CFG)/ApplFiles.mak

ifndef	StaticLibrary
Library = $(LIB)/lib$(NAME).so
endif

_Library_hdr = $(shell find -L . -name '*.h' \
	! -name '*.tst.h' ! -name '*.ext.h' -printf '%f\n')
_Library_src = $(shell find -L . -name '*.c' \
	! -name '*.tst.c' ! -name '*.ext.c' -printf '%f\n')

_Library_files = $(patsubst %.h,$(EFEU_INCLUDE)/%.h,$(_Library_hdr))
_Library_obj = $(patsubst %.c,$(BUILD)/%.o,$(_Library_src))
_Library_dep = $(patsubst %.o,%.d,$(_Library_obj))

all:: $(Library) $(StaticLibrary)

files:: $(_Library_files)

clean::
	rm -f $(Library) $(_Library_files)
	rm -rf $(BUILD)

$(Library): $(_Library_obj)
	@mkdir -p $(@D)
	$(CC) -shared -Wl,-soname,lib$(NAME).so -o $@ $(_Library_obj) $(LDFLAGS)

$(StaticLibrary): $(_Library_obj)
	@mkdir -p $(@D)
	ar -r $@ $(_Library_obj)
	
$(EFEU_INCLUDE)/%.h : %.h
	@mkdir -p $(@D)
	cp $< $@

-include $(_Library_dep)

$(BUILD)/%.o : %.c
	@mkdir -p $(@D)
	$(CC) -o $@ -c -MMD $(CPPFLAGS) $(CFLAGS) $<

# extra shared objects

_Library_ext_src = $(shell find -L . -name '*.ext.c' -printf '%f\n')

ifneq ($(_Library_ext_src),)

ifndef	EXT_LIB
EXT_LIB	= $(LIB)
endif

_Library_ext_so = $(patsubst %.ext.c,$(EXT_LIB)/%.so,$(_Library_ext_src))
_Library_ext_dep = $(patsubst %.c,$(BUILD)/%.d,$(_Library_ext_src))

all:: $(_Library_ext_so)

clean::
	rm -rf $(_Library_ext_so)

-include $(_Library_ext_dep)

$(EXT_LIB)/%.so : %.ext.c
	@mkdir -p $(@D)
	$(CC) -shared -o $@ -MMD -MF $(BUILD)/$(@F:.so=.ext.d) \
		$(CPPFLAGS) $(CFLAGS) $< $(LDFLAGS) $(EXT_LDFLAGS)

endif

# unit test with code coverage

test::
	make -C $(SRC)/utest all

test:: all

_Library_tst_src = $(shell find -L . -name '*.tst.c' -printf '%f\n')
_Library_tst_obj = $(patsubst %.c,$(BUILD)/%.o,$(_Library_tst_src))
_Library_tst_dep = $(patsubst %.o,%.d,$(_Library_tst_obj))
_Library_tst_bin = $(patsubst %.c,$(BUILD)/%,$(_Library_tst_src))

ifneq ($(_Library_ext_src),)
_Library_ext_gcov = $(patsubst %.ext.c,$(BUILD)/%.gcov.so,$(_Library_ext_src))

test:: $(_Library_ext_gcov)

$(BUILD)/%.gcov.so : $(BUILD)/%.gcov.o
	@mkdir -p $(@D)
	$(CC) -shared -o $@ $< -lgcov $(LDFLAGS) $(EXT_LDFLAGS) -lutest

$(BUILD)/%.gcov.o : %.ext.c
	@mkdir -p $(@D)
	$(CC) -o $@ -c -MMD $(CPPFLAGS) $(CFLAGS) --coverage $<

purge::
	rm -rf $(_Library_ext_gcov)
	rm -rf $(_Library_ext_gcov:.so=.gcno)
	rm -rf $(_Library_ext_gcov:.so=.gcda)

endif

ifneq ($(_Library_tst_bin),)

_Library_gcov = $(BUILD)/lib$(NAME).gcov.a
_Library_gcov_obj = $(patsubst %.o,%.gcov.o,$(_Library_obj))
_Library_gcov_dep = $(patsubst %.o,%.d,$(_Library_gcov_obj))

test:: $(_Library_gcov)

test:: $(_Library_tst_bin)

test::
	$(BIN)/efeu-test .

$(_Library_gcov): $(_Library_gcov_obj)
	ar -r $@ $(_Library_gcov_obj)

-include $(_Library_gcov_dep)
-include $(_Library_tst_dep)

$(BUILD)/%.gcov.o : %.c
	@mkdir -p $(@D)
	$(CC) -o $@ -c -MMD $(CPPFLAGS) $(CFLAGS) --coverage $<

$(_Library_tst_obj) : $(_Library_gcov)

$(BUILD)/%.tst.o : %.tst.c
	@mkdir -p $(@D)
	$(CC) -o $@ -c -MMD $(CPPFLAGS) -I. $(CFLAGS) $(TST_CFLAGS) --coverage $<

$(BUILD)/%.tst : $(BUILD)/%.tst.o
	@mkdir -p $(@D)
	$(CC) -o $@ $< $(_Library_gcov) -lgcov $(LDFLAGS) $(TST_LDFLAGS) -lutest

purge::
	rm -rf $(_Library_gcov)
	rm -rf $(_Library_gcov_obj) $(_Library_gcov_dep)
	rm -rf $(_Library_gcov_obj:.o=.gcno)
	rm -rf $(_Library_gcov_obj:.o=.gcda)
	rm -rf $(_Library_tst_bin) $(_Library_tst_dep)

endif
endif

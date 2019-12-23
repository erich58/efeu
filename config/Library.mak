# Create Library

ifndef _Library_mak_
_Library_mak_ = defined

include $(CFG)/EFEU.mak
include $(CFG)/ApplFiles.mak

Library = $(LIB)/lib$(NAME).so
_Library_hdr = $(shell find -L . -name '*.h' -printf '%f\n')
_Library_src = $(shell find -L . -name '*.c' -printf '%f\n')

_Library_files = $(patsubst %.h,$(EFEU_INCLUDE)/%.h,$(_Library_hdr))
_Library_obj = $(patsubst %.c,$(BUILD)/%.o,$(_Library_src))
_Library_dep = $(patsubst %.o,%.d,$(_Library_obj))

all:: $(Library)

files:: $(_Library_files)

clean::
	rm -f $(Library) $(_Library_files)
	rm -rf $(BUILD)

$(Library): $(_Library_obj)
	@mkdir -p $(@D)
	$(CC) -shared -Wl,-soname,lib$(NAME).so -o $@ $(_Library_obj) $(LDFLAGS)

$(EFEU_INCLUDE)/%.h : %.h
	@mkdir -p $(@D)
	cp $< $@

-include $(_Library_dep)

$(BUILD)/%.o : %.c
	@mkdir -p $(@D)
	$(CC) -o $@ -c -MMD $(CPPFLAGS) $(CFLAGS) $<

# unit test with code coverage

_Library_gcov = $(BUILD)/lib$(NAME).gcov.a

_Library_gcov_obj = $(patsubst %.o,%.gcov.o,$(_Library_obj))
_Library_gcov_dep = $(patsubst %.o,%.d,$(_Library_gcov_obj))

_Library_tst = $(shell find -L . -name '*.tst' -printf '%f\n')
_Library_bin = $(patsubst %.tst,$(BUILD)/%.bin,$(_Library_tst))
_Library_tst_dep = $(patsubst %.bin,%.bin.d,$(_Library_bin))

utest:: all $(_Library_gcov)

utest::
	make -C $(SRC)/utest

utest:: $(_Library_bin)

utest::
	printf '%s\n' $(_Library_bin) | /bin/sh

$(_Library_gcov): $(_Library_gcov_obj)
	ar -r $@ $(_Library_gcov_obj)

-include $(_Library_gcov_dep)
-include $(_Library_tst_dep)

$(BUILD)/%.gcov.o : %.c
	@mkdir -p $(@D)
	$(CC) -o $@ -c -MMD $(CPPFLAGS) $(CFLAGS) --coverage $<

$(BUILD)/%.bin : %.tst
	@mkdir -p $(@D)
	$(CC) -o $@ -MMD -MF $(@).d $(CPPFLAGS) $(CFLAGS) \
		-x c $< -x none $(_Library_gcov) -lgcov $(LDFLAGS) -lutest

purge::
	rm -rf $(_Library_gcov_obj) $(_Library_gcov_dep)
	rm -rf $(_Library_gcov_obj:.o=.gcno)
	rm -rf $(_Library_gcov_obj:.o=.gcda)
	rm -rf $(_Library_tst_bin) $(_Library_tst_dep)

endif

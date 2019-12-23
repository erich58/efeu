# Unit Test UnitTest

ifndef _UnitTest_mak_
_UnitTest_mak_ = defined

include $(CFG)/EFEU.mak

ifndef	UTEST
UTEST = .utest
endif

utest::
	make -C $(SRC)/utest

_UnitTest_src = $(shell find -L . -name '*.tst' -printf '%f\n')
_UnitTest_bin = $(patsubst %.tst,$(UTEST)/%.bin,$(_UnitTest_src))
_UnitTest_dep = $(patsubst %.bin,%.d,$(_UnitTest_bin))

utest:: all

utest:: $(_UnitTest_bin)

utest::
	printf '%s\n' $(_UnitTest_bin) | /bin/sh

purge::
	rm -rf $(UTEST)

$(UTEST)/%.bin : %.tst
	@mkdir -p $(@D)
	$(CC) -o $@ -MMD $(CPPFLAGS) $(CFLAGS) $(UTFLAGS) \
		-x c $< -x none $(LDFLAGS) -lutest

-include $(_UnitTest_dep)

endif

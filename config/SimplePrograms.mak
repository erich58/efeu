# Create Simple Programs
# Every source file generates its own program

ifndef _SimplePrograms_mak_
_SimplePrograms_mak_ = defined

include $(CFG)/EFEU.mak
include $(CFG)/ApplFiles.mak

_SimplePrograms_c = $(shell find -L . -name '*.c' -printf '%f\n')
_SimplePrograms = $(patsubst %.c,$(BIN)/%,$(_SimplePrograms_c))

all:: $(_SimplePrograms)

clean::
	rm -f $(_SimplePrograms)

$(BIN)/% : %.c
	@mkdir -p $(@D)
	$(CC) -o $@ $(CPPFLAGS) $(CFLAGS) $< $(LDFLAGS)
endif

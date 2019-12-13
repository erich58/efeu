# Create Simple Programs
# Every source file generates its own program

ifndef _SimplePrograms_mak_
_SimplePrograms_mak_ = defined

include $(CFG)/EFEU.mak

_SimplePrograms_c = $(wildcard *.c)
_SimplePrograms_cnf = $(wildcard *.cnf)

_SimplePrograms = $(patsubst %.c,$(BIN)/%,$(_SimplePrograms_c))
_SimplePrograms_files = $(patsubst %,$(LIB)/efeu/config/%,$(_SimplePrograms_cnf))

all:: $(_SimplePrograms)

files:: $(_SimplePrograms_files)

clean::
	rm -f $(_SimplePrograms) $(_SimplePrograms_files)

$(BIN)/% : %.c
	@mkdir -p $(@D)
	$(CC) -o $@ $(CPPFLAGS) $(CFLAGS) $< $(LDFLAGS)

$(LIB)/efeu/config/%.cnf : %.cnf
	@mkdir -p $(@D)
	cp $< $@

endif

# Create Complex Program
# All source files defines a single program

ifndef _ComplexProgram_mak_
_ComplexProgram_mak_ = defined

include $(CFG)/EFEU.mak

_ComplexProgram = $(BIN)/$(NAME)
_ComplexProgram_c = $(wildcard *.c)
_ComplexProgram_cnf = $(wildcard *.cnf)
_ComplexProgram_o = $(patsubst %.c,$(BUILD)/%.o,$(notdir $(_ComplexProgram_c)))
_ComplexProgram_d = $(patsubst %.o,%.d,$(_ComplexProgram_o))
_ComplexProgram_files = $(patsubst %,$(LIB)/efeu/config/%,$(_ComplexProgram_cnf))

all:: $(_ComplexProgram)

files:: $(_ComplexProgram_files)

clean::
	rm -f $(_ComplexProgram)

$(_ComplexProgram): $(_ComplexProgram_o)
	@mkdir -p $(@D)
	$(CC) -o $@ $(_ComplexProgram_o) $(LDFLAGS)

-include $(_ComplexProgram_d)

$(BUILD)/%.o : %.c
	@mkdir -p $(BUILD) $(@D)
	$(CC) -o $@ -c -MMD $(CPPFLAGS) $(CFLAGS) $<

$(LIB)/efeu/config/%.cnf : %.cnf
	@mkdir -p $(@D)
	cp $< $@

endif

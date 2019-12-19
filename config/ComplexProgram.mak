# Create Complex Program
# All source files defines a single program

ifndef _ComplexProgram_mak_
_ComplexProgram_mak_ = defined

include $(CFG)/EFEU.mak
include $(CFG)/ApplFiles.mak

_ComplexProgram = $(BIN)/$(NAME)
_ComplexProgram_src = $(shell find -L . -name '*.c' -printf '%f\n')

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

endif

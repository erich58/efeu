# Create Library

ifndef _Library_mak_
_Library_mak_ = defined

include $(CFG)/EFEU.mak
include $(CFG)/ApplFiles.mak

Library = $(LIB)/lib$(NAME).so
_Library_hdr = $(shell find -L . -name '*.h' -printf '%f\n')
_Library_src = $(shell find -L . -name '*.c' -printf '%f\n')

_Library_obj = $(patsubst %.c,$(BUILD)/%.o,$(_Library_src))
_Library_dep = $(patsubst %.o,%.d,$(_Library_obj))
_Library_files = $(patsubst %.h,$(EFEU_INCLUDE)/%.h,$(_Library_hdr))

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

endif

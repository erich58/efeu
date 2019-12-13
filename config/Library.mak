# Create Library

ifndef _Library_mak_
_Library_mak_ = defined

include $(CFG)/EFEU.mak

VPATH = $(shell find -L . -type d)

_Library = $(LIB)/lib$(NAME).so
_Library_h = $(shell find -L . -name '*.h')
_Library_c = $(shell find -L . -name '*.c')

_Library_obj = $(patsubst %.c,$(BUILD)/%.o,$(notdir $(_Library_c)))
_Library_dep = $(patsubst %.o,%.d,$(_Library_obj))
_Library_files = $(patsubst %.h,$(EFEU_INCLUDE)/%.h,$(notdir $(_Library_h)))

all:: $(_Library)

files:: $(_Library_files)

clean::
	rm -f $(_Library) $(_Library_files)
	rm -rf $(BUILD)

$(_Library): $(_Library_obj)
	@mkdir -p $(@D)
	$(CC) -shared -Wl,-soname,lib$(NAME).so -o $@ $(_Library_obj) $(LDFLAGS)

$(EFEU_INCLUDE)/%.h : %.h
	@mkdir -p $(@D)
	cp $< $@

-include $(_Library_dep)

$(BUILD)/%.d : %.c
	@mkdir -p $(@D)
	echo "$(@:.d=.o): $<" > $@

$(BUILD)/%.o : %.c
	@mkdir -p $(@D)
	$(CC) -o $@ -c -MMD $(CPPFLAGS) $(CFLAGS) $<

endif

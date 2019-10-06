include Setup.mak
include $(CFG)/EFEU.mak

LIBRARY = $(LIB)/lib$(NAME).so

LDFLAGS += -ldl
CFLAGS += -DSB_REALLOC=lrealloc

VPATH = $(shell find -L . -type d)
HCFG = $(wildcard *.hcfg */*.hcfg)
HDR = $(wildcard *.h */*.h)
SRC = $(wildcard *.c */*.c)
MSG = $(shell find . -name '*.msg')

OBJ = $(patsubst %.c,$(BUILD)/%.o,$(notdir $(SRC)))

FILES = $(patsubst %.hcfg,$(INC)/EFEU/%.h,$(notdir $(HCFG)))
FILES += $(patsubst %.h,$(INC)/EFEU/%.h,$(notdir $(HDR)))

all:: $(LIBRARY)

files:: $(FILES)

clean::
	rm -f $(LIBRARY) $(FILES)

$(LIBRARY): $(OBJ)
	@mkdir -p $(@D)
	$(CC) -shared -Wl,-soname,lib$(NAME).so -o $@ $(OBJ) $(LDFLAGS)

# update configuration header

RULES = $(patsubst %.hcfg,$(BUILD)/%.hrule,$(notdir $(HCFG)))

include $(RULES)

UPDATE = sh $(CFG)/efeu-config.sh update

$(BUILD)/%.hrule: %.hcfg
	@mkdir -p $(@D)
	printf '%s::\n\t$(UPDATE) %s $$@\n' \
		$(INC)/EFEU/$(@F:.hrule=.h) $< > $@

# installing header files

$(INC)/EFEU/%.h : %.h
	@mkdir -p $(@D)
	cp $< $@

# build object files

$(BUILD)/%.d : %.o
	@mkdir -p $(@D)
	echo '$(WORK)/cp $<' > $@

$(BUILD)/%.o : %.c
	@mkdir -p $(@D)
	$(CC) -o $@ -c -MMD $(CPPFLAGS) $(CFLAGS) $<

# installing application files

include $(CFG)/ApplFiles.mak

include Setup.mak
include $(CFG)/EFEU.mak

LIBRARY = $(LIB)/lib$(NAME).so

LDFLAGS += -lefx -lefm -ldl -lm

all:: $(LIBRARY)

clean::
	rm -f $(LIBRARY)

$(LIBRARY): $(OBJ)
	@mkdir -p $(@D)
	$(CC) -shared -Wl,-soname,lib$(NAME).so -o $@ $(OBJ) $(LDFLAGS)

include $(CFG)/ApplFiles.mak

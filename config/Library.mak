# Create Library

ifndef _Library_mak_
_Library_mak_ = defined

include $(CFG)/EFEU.mak

LIBRARY = $(LIB)/lib$(NAME).so

all:: $(LIBRARY)

clean::
	rm -f $(LIBRARY)

$(LIBRARY): $(OBJ)
	@mkdir -p $(@D)
	$(CC) -shared -Wl,-soname,lib$(NAME).so -o $@ $(OBJ) $(LDFLAGS)

endif

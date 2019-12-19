# Magic Files

ifndef _MagicFiles_mak_
_MagicFiles_mak_ = defined

include $(CFG)/EFEU.mak

ifndef	MAGIC
MAGIC = $(ETC)/efeu
endif

_MagicFiles_src = $(shell find . -name '*.magic' -printf '%f\n')
_MagicFiles_files = $(patsubst %.magic,$(MAGIC)/magic.d/%,$(_MagicFiles_src))

files:: $(_MagicFiles_files)

clean::
	rm -f $(_MagicFiles_files)

$(MAGIC)/magic.d/% :%.magic
	@mkdir -p $(@D)
	cp $< $@

all clean::
	-$(BIN)/efeu-magic $(MAGIC)

endif

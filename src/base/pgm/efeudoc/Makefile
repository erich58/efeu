TOP=../../../..
include $(TOP)/Setup.mak
include $(CFG)/CC.mak

EFEU_INCLUDE = $(INC)/efeudoc
CPPFLAGS += -I$(EFEU_INCLUDE)

include $(CFG)/EFEU.mak
include $(CFG)/ApplFiles.mak

prereq::
	$(MAKE) -C ../../scripts
	$(MAKE) -C ../esh

LDFLAGS += -lefmath -liorl -lmd -lefx -lefm -ldl -lm
VPATH = $(shell find -L . -type d)
HDR = $(wildcard *.h */*.h)
HFILES = $(patsubst %,$(EFEU_INCLUDE)/%,$(notdir $(HDR)))

DBUF_SRC = $(wildcard DocBuf/*.c)
DBUF_OBJ = $(patsubst %.c,$(BUILD)/%.o,$(notdir $(DBUF_SRC)))

EM_SRC = $(wildcard efeuman/*.c)
EM_OBJ = $(patsubst %.c,$(BUILD)/%.o,$(notdir $(EM_SRC)))

S2D_SRC = $(wildcard src2doc/*.c)
S2D_OBJ = $(patsubst %.c,$(BUILD)/%.o,$(notdir $(S2D_SRC)))

DOC_SRC = $(wildcard *.c sym/*.c tab/*.c term/*.c mroff/*.c latex/*.c html/*.c)
DOC_OBJ = $(patsubst %.c,$(BUILD)/%.o,$(notdir $(DOC_SRC)))

DEP = $(patsubst %.o,%.d,$(DBUF_OBJ) $(EM_OBJ) $(S2D_OBJ), $(DOC_OBJ))

-include $(DEP)

PGM=	$(BIN)/efeuman $(BIN)/src2doc $(BIN)/efeudoc

all:: $(PGM)

clean::
	rm -f $(PGM)

$(BIN)/efeuman: $(EM_OBJ) $(DBUF_OBJ)
	$(CC) -o $@ $(EM_OBJ) $(DBUF_OBJ) $(LDFLAGS)

$(BIN)/src2doc: $(S2D_OBJ) $(DBUF_OBJ)
	$(CC) -o $@ $(S2D_OBJ) $(DBUF_OBJ) $(LDFLAGS)

$(BIN)/efeudoc: $(DOC_OBJ)
	$(CC) -o $@ $(DOC_OBJ) $(LDFLAGS)

$(BUILD)/%.d : %.c
	echo "$(@:.d=.o): $<" > $@

$(BUILD)/%.o : %.c
	$(CC) -o $@ -c -MMD $(CPPFLAGS) $(CFLAGS) $<

files:: $(HFILES)

clean::
	rm -f $(HFILES)

$(EFEU_INCLUDE)/% : %
	@mkdir -p $(@D)
	cp $< $@

# Scripts and Files

CFGDIR=$(LIB)/efeudoc
ESHLIB=$(LIB)/esh

FILES  = $(patsubst %.esh,$(BIN)/%,$(notdir $(wildcard *.esh */*.esh)))
FILES += $(patsubst %,$(ESHLIB)/%,$(notdir $(wildcard *.hdr */*.hdr)))
FILES += $(patsubst %,$(CFGDIR)/%,$(notdir $(wildcard *.sym */*.sym)))
FILES += $(patsubst %,$(CFGDIR)/%,$(notdir $(wildcard *.dmac */*.dmac)))
FILES += $(patsubst %,$(CFGDIR)/%,$(notdir $(wildcard *.cfg */*.cfg)))
FILES += $(patsubst %,$(CFGDIR)/%,$(notdir $(wildcard *.eps */*.eps)))
FILES += $(patsubst %,$(CFGDIR)/%,$(notdir $(wildcard *.ltx */*.ltx)))

files:: $(CFGDIR) $(ESHLIB) $(FILES)

$(CFGDIR) $(ESHLIB):
	mkdir -p $@

$(BIN)/% : %.esh
	$(BIN)/efeuscript -e -c esh $< $@

$(ESHLIB)/% : %
	cp $< $@

$(CFGDIR)/% : %
	cp $< $@

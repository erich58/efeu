# :*: document generator
# :de: Dokumentgenerator

TOP=../../../..
include $(TOP)/Setup.mak
include $(CFG)/CC.mak

EFEU_INCLUDE = ../include
CPPFLAGS += -I$(EFEU_INCLUDE)

include $(CFG)/EFEU.mak
include $(CFG)/ApplFiles.mak
include $(CFG)/Scripts.mak

prereq::
	make -C ../DocBuf

LDFLAGS += -L ../lib -lDocBuf
LDFLAGS += -lefmath -liorl -lmd -lefx -lefm -ldl -lm

include $(CFG)/ComplexProgram.mak

# Extra Files

CFGDIR=$(LIB)/efeudoc
ESHLIB=$(LIB)/esh

FILES = $(patsubst %,$(ESHLIB)/%,$(notdir $(wildcard *.hdr */*.hdr)))
FILES += $(patsubst %,$(CFGDIR)/%,$(notdir $(wildcard *.sym */*.sym)))
FILES += $(patsubst %,$(CFGDIR)/%,$(notdir $(wildcard *.dmac */*.dmac)))
FILES += $(patsubst %,$(CFGDIR)/%,$(notdir $(wildcard *.cfg */*.cfg)))
FILES += $(patsubst %,$(CFGDIR)/%,$(notdir $(wildcard *.eps */*.eps)))
FILES += $(patsubst %,$(CFGDIR)/%,$(notdir $(wildcard *.ltx */*.ltx)))

files:: $(FILES)

clean::
	rm -f $(FILES)

clean::
	rmdir --ignore-fail-on-non-empty -p $(ESHLIB) $(CFGDIR)

$(ESHLIB)/% : %
	@mkdir -p $(@D)
	cp $< $@

$(CFGDIR)/% : %
	@mkdir -p $(@D)
	cp $< $@

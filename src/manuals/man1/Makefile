# :*: efeu basic documentation
# :de: EFEU Basishandbücher

TOP=../../..
include $(TOP)/Setup.mak
include $(CFG)/Config.mak

CMDLIST = dircmp dircpy edb edbcat edbjoin edbtrans efeu-magic efeu-xlib \
	efeucc efeudoc efeuscript efile esh findgrep htmlindex \
	mdcat mdcmp mdcreate mddiag mddiff mdfile mdfunc mdmul mdpaste \
	mdpermut mdprint mdread mdround mdselect mdsum mdterm \
	mkmf mksource mksrclist pp2dep shmkmf shmkmf-config src2doc tex2ps

EN_MAN = $(patsubst %,$(MAN)/man1/%.1.gz,$(CMDLIST))
DE_MAN = $(patsubst %,$(MAN)/de/man1/%.1.gz,$(CMDLIST))

FILES = $(EN_MAN) $(DE_MAN)

doc:: $(FILES)

clean::
	rm -f $(FILES)

$(MAN)/man1/%.1.gz: $(BIN)/%
	@mkdir -p $(@D)
	LANG=en_US $< --help=man | gzip > $@

$(MAN)/de/man1/%.1.gz: $(BIN)/%
	@mkdir -p $(@D)
	LANG=de_AT $< --help=man | gzip > $@


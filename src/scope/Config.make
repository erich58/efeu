# :*: rules for tag files and name tables
# :de: Sourcelisten und TAG-File zusammenstellen
##Depends: base

include efeu.smh
include usage.smh

mf_var	BDIR $TOP/build

mf_var	BASE '$(BDIR)/std.d $(BDIR)/synt.d $(BDIR)/wmm.d $(BDIR)/oestz.d'

mf_rule -d all
mf_rule -d update "" "rm -f Base" "make all"
mf_rule -c -d Base "" "rm -f List" "-mksrclist \$(BDIR) \$@"

mf_rule -c -d List Base \
	"-grep List Base | xargs sort -u > \$@" \
	"-grep d2m.list Base | xargs sort -u >> \$@" \
	"-grep '\\.[ch]\$\$' Base | sort -u >> \$@" \
	"-grep '\\.[0-9]\$\$' Base | sort -u >> \$@" \

mf_rule	-a C List "-grep '\\.c\$\$' List > \$@"
mf_rule -a H List "-grep '\\.h\$\$' List > \$@"
mf_rule -a SRC List "-grep '\\.src\$\$' List > \$@"
mf_rule -a TPL List "-grep '\\.tpl\$\$' List > \$@"
mf_rule -a DEF List "-grep '\\.def\$\$' List > \$@"
mf_rule -a MS List "-grep '\\.ms\$\$' List > \$@"
mf_rule -a ESH List "-grep '\\.esh\$\$' List > \$@"
mf_rule -a MSG List "-grep '\\.msg\$\$' List > \$@"
mf_rule -a CNF List "-grep '\\.cnf\$\$' List > \$@"
mf_rule -a DOC List "-grep '\\.doc\$\$' List > \$@"
mf_rule -a MAN List "-grep '\\.[0-9]\$\$' List > \$@"
mf_rule -a CM List "-grep 'Config.make\$\$' List > \$@"

xinclude $SRC/ctags.smh ctags.stat
efeu_note -d ctags

if
	grep -q CTAGS ctags.stat
then
	mf_comm	"tag file for vi(m)"
	mf_rule -a -d tags "" 'rm -f $@' 'cat C H | xargs $(CTAGS)'
else
	efeu_note ctags << !
The command ctags is not available, so no tag file is created.
This is not needed for running efeu-commands, but tag commands
could not be used to jump to the definition of a specific symbol
on visiting the sources.
!
fi

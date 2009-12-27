include efeu.smh

mf_dir TEXMF=$TOP/share/texmf
mf_dir 'TEXDIR=$(TEXMF)/tex/latex/misc'
foreach -m TEXDIR -S sty 'mf_file $tg $src'
mf_rule -d all "" '-texhash $(TEXMF)'


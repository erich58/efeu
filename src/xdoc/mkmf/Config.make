# :*: Makefile generation
# :de: Generieren von Makefiles
##Depends: .

include efeu.smh

AllTarget=files
foreach -S c 'mf_file $tg $src'
foreach -S h 'mf_file $tg $src'
foreach -S tex 'mf_file $tg $src'
foreach -S config 'mf_file $tg $src'

AllTarget=all
foreach -S c 'mf_cc -c $src'
mf_objlist OBJ
mf_cc -o pasctrian -mo OBJ
mf_rule -a tab.tex pasctrian \
	"./pasctrian 12 '\C{%d}' | sed -e 's/\$\$/\\\\\\\\/' > %@"

mf_rule -d doc "" 'efeucheck latex && $(MAKE) docfiles'
mf_rule -A docfiles -c pasctrian.dvi "pasctrian.tex tab.tex" "latex %1"
mf_rule -A docfiles -c pasctrian.ps pasctrian.dvi "dvips -o %@ %1"
mf_clean pasctrian.aux pasctrian.log

efeu_doc -l "de_AT" mkmf
#efeu_doc -l "de_AT en_US" mkmf


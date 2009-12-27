# :en: german documentation about EFEU programming
# :de: EFEU Programmierhandbuch
##Depends: .

include efeu.smh
APPLDIR=.
mf_rule -d all commands
mf_rule -d commands
foreach -A commands -s c 'mf_cc -o $tg $src -lefm'
foreach -A commands -s esh 'mf_script -e -c esh $tg $src'
foreach -A commands -s tpl 'efeu_gensrc -p $src'
efeu_doc -l de prog

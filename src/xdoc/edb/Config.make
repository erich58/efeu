include efeu.smh
efeu_doc -l "de en" edb
foreach -s esh 'mf_script -e -c esh $tg $src'
foreach -S edb 'mf_file $tg $src'


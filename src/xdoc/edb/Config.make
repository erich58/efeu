include efeu.smh
AllTarget=files
foreach -s esh 'mf_script -e -c esh $tg $src'
foreach -S edb 'mf_file $tg $src'
foreach -S hdr 'mf_file $tg $src'
foreach -S def 'mf_file $tg $src'
AllTarget=all
efeu_doc -l "de en" edb


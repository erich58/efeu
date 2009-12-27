include efeu.smh
foreach -p $TOP/lib/shmkmf -S smh 'mf_file $tg $src'
foreach -p $TOP/ppinclude -S pph 'mf_config -u $tg $src'

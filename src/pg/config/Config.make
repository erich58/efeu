include efeu.smh
mf_dir -q $TOP/include/DB
foreach -p $TOP/lib/shmkmf -S smh 'mf_file $tg $src'

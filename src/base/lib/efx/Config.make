include efeu.smh
include dl.smh
efeu_library -c -h EFEU efx -ml LD_DL

mf_dir MAGIC=$TOP/etc/efeu/magic.d
foreach -m MAGIC -s magic 'mf_file $tg $src'
mf_rule -d all "" "-efeu-magic $TOP/etc/efeu"

# :en: extension library
# :de: Erweiterungsbibliothek
##Depends: efm

include efeu.smh
efeu_library -c -h EFEU efx -ml EFM_LIB -lm

mf_dir MAGIC=$TOP/etc/efeu/magic.d
foreach -m MAGIC -s magic 'mf_file $tg $src'
mf_rule -d all "" "-efeu-magic $TOP/etc/efeu"

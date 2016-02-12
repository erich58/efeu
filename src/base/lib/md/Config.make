# :*: data cube library
# :de: Datenmatrix
##Depends: efm efx

include efeu.smh
efeu_library -h EFEU md -ml EFX_LIB

mf_dir MAGIC=$TOP/etc/efeu/magic.d
foreach -m MAGIC -s magic 'mf_file $tg $src'
mf_rule -d all "" "-efeu-magic $TOP/etc/efeu"

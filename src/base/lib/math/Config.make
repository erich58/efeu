include efeu.smh
efeu_library -h Math efmath -lmd -ml EFX_LIB
mf_dir TSHDR=$TOP/lib/esh/TimeSeries
foreach -m TSHDR -s hdr 'mf_file $tg $src'

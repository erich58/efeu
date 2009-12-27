include efeu.smh
include readline.smh
mf_dir BIN=$TOP/bin
efeu_libvar iorl -ml RL_LIB efx efm -lm
foreach -m BIN -s c 'mf_cc -md DEP -o $tg $src -ml LIB'

include efeu.smh
include readline.smh
include X11.smh

mf_dir -p BIN=$TOP/bin
efeu_libvar pix iorl -ml RL_LIB md efmath efx efm -lm
mf_cc -o '$(BIN)/pixmap' $SRC/pixmap.c -md DEP -ml LIB
mf_cc -o '$(BIN)/pixtoX11' $SRC/pixtoX11.c -md DEP -ml LIB -ml LD_X11
foreach -m BIN -s pix 'mf_script -e -c pixmap $tg $src'

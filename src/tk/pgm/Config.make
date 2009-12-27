include efeu.smh
include readline.smh
xinclude tklib.smh

mf_dir -p BIN=$TOP/bin
efeu_libvar -p ETK ETK -ml TK_LIB iorl -ml RL_LIB md efmath efx efm -lm
mf_var CFLAGS	'$(TK_INC)'
mf_cc -o '$(BIN)/etk' $SRC/etk.c -md DEP_ETK -ml LIB_ETK

# :de: Hilfsprogramme für Pixeldateien
##Depends: pix

include efeu.smh
include readline.smh
include X11.smh

mf_dir -p BIN=$TOP/bin

efeu_libvar -p PIX pix iorl -ml RL_LIB md efmath efx efm -lm
mf_cc -o '$(BIN)/pixmap' $SRC/pixmap.c -md DEP_PIX -ml LIB_PIX

if	[ "$LD_LIB" ]; then
	mf_cc -o '$(BIN)/pixtoX11' $SRC/pixtoX11.c -md DEP_PIX -ml LIB_PIX -ml LD_X11
else
	efeu_note pixtoX11 <<!
The building of the command pixtoX11 were rejected due to missing
X11 development files. Install the corresponding package(s) and
rebuild the library.
!
fi

foreach -m BIN -s pix 'mf_script -e -c pixmap $tg $src'

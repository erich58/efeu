include efeu.smh
xinclude $SRC/efwin.smh

foreach -A files -d . -S h 'mf_file $tg $src'
foreach -A files -d . -s hcfg 'mf_config -u $tg.h $src'

efeu_libvar -p EIS -ml LD_EFWIN efx efm -lm -ml LD_CURSES
foreach -s c 'mf_cc -I. -c $src' $SRC/eistty
mf_objlist OBJ_EIS
mf_cc -o $TOP/bin/eis -mo OBJ_EIS -md DEP_EIS -ml LIB_EIS

add_depend eistty.h
efeu_note -d eis

if
	grep -q "USE_EFWIN.*0" eistty.h
then
	efeu_note eis << !
The command eis is build without curses support because
libefwin is not available.
!
fi

efeu_libvar -p CGI efm
foreach -s c 'mf_cc -c $src' $SRC/eis2html
mf_objlist OBJ_CGI
mf_dir -q $TOP/cgi-bin
mf_cc -o $TOP/cgi-bin/eis-cgi -mo OBJ_CGI -md DEP_CGI -ml LIB_CGI

mf_dir EISDIR=$TOP/lib/eis
foreach -m EISDIR -M de -s eis 'mf_file $tg.info $src'

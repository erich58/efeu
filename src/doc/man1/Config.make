# :en: manpages for commands
# :de: Handbucheinträge zu Kommandos
##Depends: .

include efeu.smh

mf_var BIN $TOP/bin
mf_dir MAN_en=$TOP/man/man1
mf_dir MAN_de=$TOP/man/de/man1

case `uname -s` in
AIX)	ext=""; filter="";;
*)	ext=".gz"; filter="| gzip";;
esac

manentry ()
{
	for lng in en de
	do
		mf_rule -a "\$(MAN_$lng)/$1.1$ext" "\$(BIN)/$1" \
			"LANG=$lng $1 --help=man $filter > \$@"
	done
}

manentry dircmp
manentry dircpy
manentry edb
manentry edbcat
manentry edbjoin
manentry edbtrans
manentry efeu-magic
manentry efeu-xlib
manentry efeucc
manentry efeudoc
manentry efeuscript
manentry efile
manentry esh
manentry findgrep
manentry htmlindex
manentry mdcat
manentry mdcmp
manentry mdcreate
manentry mddiag
manentry mddiff
manentry mdfile
manentry mdfunc
manentry mdmul
manentry mdpaste
manentry mdpermut
manentry mdprint
manentry mdread
manentry mdround
manentry mdselect
manentry mdsum
manentry mdterm
manentry mkmf
manentry mksource
manentry mksrclist
manentry pp2dep
manentry shmkmf
manentry shmkmf-config
manentry src2doc
manentry tex2ps

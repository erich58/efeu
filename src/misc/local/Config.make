# :de: Lokale Ergänzungen
##Depends: .

include efeu.smh
mf_dir BIN=$TOP/bin
foreach -m BIN -s tcl 'mf_script -e -c wish $tg $src'

mf_dir DATA=$TOP/share/eletters
mf_dir IMAGES='$(DATA)/images'
mf_dir AUDIO='$(DATA)/audio'

foreach -m AUDIO -M "de *=en" -S au 'mf_file $tg $src'
foreach -m IMAGES -S gif 'mf_file $tg $src'

LinkImage ()
{
	dir="\$(DATA)/$1"
	mf_rule -a "$dir/$2" "\$(DATA)/images/$3" \
		"test -d $dir || mkdir -p $dir" \
		"rm -f \$@; ln -fs \$(DATA)/images/$3 \$@"
}

LinkImage de/A Anneliese.gif Anneliese.gif 
LinkImage de/E Erich.gif Erich.gif 
LinkImage de/L Liselotte.gif Liselotte.gif 
LinkImage de/U Ulrich.gif Ulrich.gif 
LinkImage de/P Der_Pauli.gif Pauli.gif 
LinkImage de/W Der_Wuffi.gif Wuffi.gif 

# :en: EFEU Letters and Numbers
# :de: EFEU-Version von Buchstaben und Zahlen
##Depends: .

include efeu.smh
mf_dir BIN=$TOP/bin
foreach -m BIN -s etk 'mf_script -e -c etk $tg $src'

mf_dir DATA=$TOP/share/eletters
mf_dir IMAGES='$(DATA)/images'
mf_dir AUDIO='$(DATA)/audio'

foreach -m IMAGES -S gif 'mf_file $tg $src'

LinkImage ()
{
	dir="\$(DATA)/$1"
	mf_rule -a "$dir/$2" "\$(DATA)/images/$3" \
		"test -d $dir || mkdir -p $dir" \
		"rm -f \$@; ln -fs \$(DATA)/images/$3 \$@"
}

foreach -s rules 'include $src'

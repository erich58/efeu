# :en: shell scripts
# :de: Shell-Scripts
##Depends: .

include efeu.smh
mf_var -x BIN $TOP/bin
mf_dir $BIN
foreach -m BIN -s sh 'mf_script -s "|:VN:|$EFEU_VERSION|g" $tg $src'
mf_rule -a $BIN/tex2pdf "" "(cd $BIN; ln -fs tex2ps tex2pdf)"

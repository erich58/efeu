# :en: vim configuration files
# :de: vim Konfigurationsdateien
##Depends: .

include efeu.smh
mf_dir BIN=$TOP/bin VIMDIR=$TOP/share/vim -p $TOP/share/vim/syntax

vim_file ()
{
	mf_rule -a "$1" "$2" "sed -e 's|VIMDIR|\$(VIMDIR)|g' $2 > \$@"
}

foreach -m VIMDIR -S vimrc 'vim_file $tg $src'
foreach -m VIMDIR -M syntax -S vim 'mf_file $tg $src'
foreach -m BIN -s sh 'mf_script $tg $src'

mf_rule -d all '$(BIN)/efeu-vimrc' '$(BIN)/efeu-vimrc $(VIMDIR)'

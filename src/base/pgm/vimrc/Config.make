# :en: vim configuration files
# :de: vim Konfigurationsdateien
##Depends: .

include efeu.smh
mf_dir BIN=$TOP/bin VIMDIR=$TOP/share/vim -p $TOP/share/vim/syntax

foreach -m VIMDIR -S vimrc "mf_file -s '|VIMDIR|\$(VIMDIR)|g' \$tg \$src"
foreach -m VIMDIR -M syntax -S vim 'mf_file $tg $src'
foreach -m BIN -s sh 'mf_script -s "|:VN:|$EFEU_VERSION|" $tg $src'

mf_rule -d all '$(BIN)/efeu-vimrc' '$(BIN)/efeu-vimrc $(VIMDIR)'

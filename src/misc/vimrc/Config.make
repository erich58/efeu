# :de: vim-Konfigurationsdateien
##Depends: .

include efeu.smh
mf_dir VIM=$TOP/share/vim
foreach -m VIM -S vimrc "mf_file \$tg \$src \"sed -e 's|VIM|\\\$(VIM)|g'\""
foreach -m VIM -S txt 'mf_file $tg $src'

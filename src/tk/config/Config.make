# :*: configuration of TCL/TK-libraries
# :de: Konfiguration für TCL/TK-Bibliotheken
##Depends: .

include efeu.smh
mf_dir -p $TOP/include/GUI
foreach -p $TOP/lib/shmkmf -S smh 'mf_file $tg $src'
foreach -p $TOP/ppinclude -S pph 'mf_config -u $tg $src'

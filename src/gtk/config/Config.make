# :en: configuration of Gtk+
# :de: Konfiguration f�r Gtk+
##Depends: .

include efeu.smh
foreach -p $TOP/lib/shmkmf -S smh 'mf_file $tg $src'

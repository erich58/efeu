include efeu.smh

mf_dir MAN=$TOP/man
mf_dir DE_MAN=$TOP/man/de

for x in 3 7
do
	mf_dir MAN_$x="\$(MAN)/man$x"
	mf_dir DE_MAN_$x="\$(DE_MAN)/man$x"
done

base=$TOP/src/base

foreach -m DE_MAN_7 -s ms 'efeu_man -l de -s 7 $tg $src' $base/pgm/esh
foreach -m DE_MAN_3 -s c 'efeu_man -l de -s 3 $tg $src' $base/lib/efm
foreach -m DE_MAN_7 -s h 'efeu_man -l de -s 7 $tg $src' $base/lib/efm

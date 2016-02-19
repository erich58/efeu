# :*: manpages to the libraries
# :de: Handbücher zu den Programmbiblitheken
##Depends: .

include efeu.smh
ldir=`cd $SRC/../lib; pwd`

foreach -p $TOP/man/de/man3 -s c 'efeu_man -l de_AT -s 3 $tg $src' $ldir/efm
foreach -p $TOP/man/de/man7 -s h 'efeu_man -l de_AT -s 7 $tg $src' $ldir/efm

efeu_doc -l "de_AT" efm $ldir/efm
efeu_doc -l "de_AT" iorl $ldir/iorl
efeu_doc -l "de_AT" md $ldir/md

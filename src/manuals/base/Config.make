# :*: manpages to the libraries
# :de: Handbücher zu den Programmbiblitheken
##Depends: .

include efeu.smh
ldir=`cd $TOP/src/base/lib; pwd`

mf_srclist efm $ldir/efm
foreach -p $TOP/man/de/man3 -s c 'efeu_man -l de_AT -s 3 $tg $src'
foreach -p $TOP/man/de/man7 -s h 'efeu_man -l de_AT -s 7 $tg $src'

efeu_doc -l "de_AT" efm $ldir/efm
efeu_doc -l "de_AT" iorl $ldir/iorl
efeu_doc -l "de_AT" md $ldir/md

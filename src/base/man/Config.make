# :*: manpages to the libraries
# :de: Handbücher zu den Programmbiblitheken
##Depends: efeudoc

include efeu.smh
ldir=`cd $SRC/../lib; pwd`

efeu_doc -l "de_AT" efm $ldir/efm
efeu_doc -l "de_AT" iorl $ldir/iorl
efeu_doc -l "de_AT" md $ldir/md

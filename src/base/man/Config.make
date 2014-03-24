# :en: manpages to the libraries
# :de: Handbücher zu den Programmbiblitheken
##Depends: doc

include efeu.smh
ldir=`cd $SRC/../lib; pwd`

efeu_doc -l "de" efm $ldir/efm
efeu_doc -l "de" iorl $ldir/iorl
efeu_doc -l "de" md $ldir/md

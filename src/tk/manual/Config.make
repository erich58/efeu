# :*: documentation
# :de: Dokumentation
##Depends: pgm

include efeu.smh
mf_var LIB $SRC/../lib

#efeu_doc -l "en_US de_AT" -H '@LIB=$(LIB)' tk $SRC
efeu_doc -l "en_US de_AT" -H '@LIB=$(LIB)' tk $SRC

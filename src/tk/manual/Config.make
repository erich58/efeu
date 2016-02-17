# :*: documentation
# :de: Dokumentation
##Depends: pgm

include efeu.smh
mf_var LIB $SRC/../lib
#mf_dir DE_DOC=$TOP/doc/de
#mf_dir EN_DOC=$TOP/doc/en

efeu_doc -l "en de" -H '@LIB=$(LIB)' tk $SRC

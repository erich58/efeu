# :*: Supplemented data types for interpreter
# :de: Ergänzende Datentypen für Interpreter
##Depends: efm efx esh

CC=efeucc
include so.smh

mf_dir -A files -x INC=$TOP/include/EfiType
mf_dir -x LIB=$TOP/lib/esh/types

foreach -m INC -S h 'mf_file $tg $src'
foreach -m LIB -s c 'so_cc $tg $src'
foreach -s tpl 'so_mksource -H $INC -L $LIB $src $libs'

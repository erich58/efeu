# :*: PostgreSQL-interface
# :de: Schnittstelle zu PostgreSQL
##Depends: config

xinclude libpq.smh
include efeu.smh

mf_var CFLAGS '$(PQ_INC)'
efeu_library -x -u -h DB PG -d libpq.stat -lmd -ml EFX_LIB -ml PQ_LIB

foreach -p $TOP/lib/esh -S hdr 'mf_file $tg $src'

if [ "$cc_shared" ]; then
mf_dir -q $TOP/lib/esh/types
mf_rule -a $TOP/lib/esh/types/PG.so '$(LIB)' "ln -s %1 %@"
mf_rule -a $TOP/lib/esh/types/PG_Grp.so '$(LIB)' "ln -s %1 %@"
mf_rule -a $TOP/lib/esh/types/PGCube.so '$(LIB)' "ln -s %1 %@"
fi

efeu_doc -l "de_AT en_US" pg

# check the status of libpq availability

HDR=$TOP/include/DB/pq_config.h

efeu_test PG "grep 'HAS_PQ.*1' $TOP/include/DB/pq_config.h" <<!
PostgreSQL interface for EFEU ist not available because PostgreSQL
development files are not installed or on an unexpected place.
Correct this and rebuild the module.
!

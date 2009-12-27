include efeu.smh
xinclude libpq.smh

mf_var CFLAGS '$(PQ_INC)'
efeu_library -u -h DB PG -d libpq.stat -lmd -ml EFX_LIB -ml PQ_LIB

foreach -p $TOP/lib/esh -S hdr 'mf_file $tg $src'

efeu_doc -l "de en" pg


# check the status of libpq availability

HDR=$TOP/include/DB/pq_config.h

add_depend $HDR
efeu_note -d PG

test -f $HDR || return

if
	grep "HAS_PQ.*0" $HDR
then
	efeu_note PG <<!
PostgreSQL interface for EFEU ist not available because PostgreSQL
development files are not installed or on an unexpected place.
Correct this and rebuild the module.
!
fi

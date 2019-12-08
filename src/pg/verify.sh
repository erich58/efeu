# check the status of libpq availability

HDR=$TOP/include/DB/pq_config.h

if	grep 'HAS_PQ.*0' $2
then
	tee $1 <<!
PostgreSQL interface for EFEU ist not available because PostgreSQL
development files are not installed or on an unexpected place.
Correct this and rebuild the module.
!
else
	rm -vf $1
fi

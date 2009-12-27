if
	test $# -ne 1
then
	echo "Aufruf $0 value"
	exit 1
fi

awk "
BEGIN { flag = 1 }
/^%/ { print; next }
{
	if	(flag)
		print \"{$1 exp} dup dup currenttransfer setcolortransfer\"

	flag = 0;
	print;
}
"

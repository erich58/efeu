df | awk '
BEGIN {
	Blksize = 1024
#	MByte = 1048576
	MByte = 1000000
	CONST = Blksize / MByte
#	CONST = 1. / 1000.
	sb = 0
	su = 0
	sf = 0
}
/^File/ {
	printf "%-14s", "Filesystem"
	printf " %10s %13s %13s", "Gesamt MB", "Verbraucht", "Verfügbar"
	printf "  Mountpunkt\n"
	next
}
{
	printf "%-16s", $1
	b = $2 * CONST
	u = $3 * CONST
	f = $4 * CONST
#	f = b - u
	sb += b
	sf += f
	su += u
	printf " %8.2f %8.2f %3.0f%%", b, u, 100 * (u / b)
	printf " %8.2f %3.0f%%  %s\n", f, 100 * (f / b), $6
}
END {
	printf "%-16s", "Insgesamt"
	printf " %8.2f %8.2f %3.0f%%", sb, su, 100 * (su / sb)
	printf " %8.2f %3.0f%%\n", sf, 100 * (sf / sb)
}
'

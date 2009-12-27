#	gdb - Initialisierung generieren

src=/home/efeu/src/lib
list="efm efx md math synt"

#	Aufarbeiten der Argumentliste

usage ()
{
	cat <<!
Aufruf: $0 -h
!
}

if
	set -- `getopt hp "$@" 2>&1`
	test $? -ne 0
then
	echo $*
	usage
	exit 1
fi

for i
do
	case $i in
	-h)	shift; usage;;
	-p)	shift; list="$list pnom";;
	--)	shift; break;;
	esac
done

tmp=/usr/tmp/gg$$

for i in $list
do
	cat /dev/null > $tmp

	for x in `find $src/$i -type d -print`
	do
		if
			fgrep -q $x $tmp 
		then
			continue
		else
			echo "directory $x"
		fi

		if
			test -f $x/LOCALFILES
		then
			sed	-e '/^[ 	]*#/d'	\
				-e '/^[ 	]*$/d'	\
				-e "s|^[ 	]*|$x/|" \
				$x/LOCALFILES >> $tmp
		fi
	done
done

rm -f $tmp
exit 0

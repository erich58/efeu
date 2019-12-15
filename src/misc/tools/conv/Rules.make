linkfile ()
{
	while
		read dir src tg
	do
		mf_rule -a $dir/$tg $dir/$src \
			"cd $dir; rm -f $tg; ln -fs $src $tg"
	done
}

linkfile << EOF
\$(BIN)	d2u u2d
$APPLDIR/help d2u.hlp u2d.hlp
$APPLDIR/de/help d2u.hlp u2d.hlp
EOF

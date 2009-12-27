if
	test $# -lt 1
then
	exec eistty top
else
	exec eistty "$@"
fi

#if
#	test "$DISPLAY"
#then
#	exec grail /home/efeu/eis/index.html
#else
#	exec lynx /home/efeu/eis/index.html
#fi

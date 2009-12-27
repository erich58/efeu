#	AWK-Skript zum Einbau systemspezifischer Parameter in Headerdateien
#	(c) 1999 Erich Frühstück
#	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5

/^@eval/ {
	gsub("@eval", eval)
	system($0)
	next
}

{
	print
}

#	AWK-Skript zum Einbau systemspezifischer Parameter in Headerdateien
#	(c) 1999 Erich Fr�hst�ck
#	A-3423 St.Andr�/W�rdern, S�dtirolergasse 17-21/5

/^@eval/ {
	gsub("@eval", eval)
	system($0)
	next
}

{
	print
}

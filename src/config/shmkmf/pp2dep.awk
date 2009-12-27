BEGIN {
	FS = "\"";
}
/^#/ {
	if	(NF != 3)			next;
	if	(match($1, "pragma"))		next;
	if	(match($2, "^<"))		next;
	if	(match($2, "/$"))		next;
	if	(match($2, "^/usr/include/"))	next;
	if	(match($2, "^/usr/lib/"))	next;

	tab[$2];
}

END {
	if (tg)
	{
		printf("%s:", tg);
		n = length(tg) + 1;
	}
	else	n = 0;

	for (x in tab)
	{
		k = length(x) + 1;

		if (n + k > 75)
		{
			printf(" \\\n");
			n = 0;
		}

		printf(" %s", x);
		n += k;
	}

	printf("\n");
}

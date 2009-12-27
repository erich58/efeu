/*	Labeldefinitionen
	(c) 1996 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

#ifndef	_TM_TABLABEL
#define	_TM_TABLABEL	1

struct Index_t {
	int num;	/* Nummer */
	int width;	/* Breite */
	str special;	/* Sonderbefehle */
	str name;	/* Kennung */
	str label;	/* Bezeichnung */
};

Index_t int ()
	return { this };

virtual int fprint(IO io, Index_t idx)
	fprintf(io, "\t%d: %#s, %d, %#s\n", this.num, this.name, this.width, this.label)


struct TabLabel {
	str key;	/* Kennung */
	str name;	/* Achsenname */
	str list;	/* Selektionsliste */
	DataBase tab;	/* Indextabelle */
	str special;	/* Sonderdefinitionen */
};

int CompLabel(TabLabel a, TabLabel b)
	cmp(a.key, b.key)

bool TabLabel ()
	this.key ? true : false;

str TabLabel ()
	paste("=", this.name, this.list)

TabLabel _Ptr_ ()
	return { NULL };

virtual int fprint(IO io, TabLabel label)
{
	if	(label.key == NULL)
		return fprint(io, "NULL");

	int n = fprintf(io, "%s[%s] = {\n", label.key, (str) label);

	for (x in label.tab)
		fprintf(io, "\t%s, %d, %#s\n", x.name, x.width, x.label)

	return n + fprintf(io, "}\n");
}


/*	Konstruktionshilfsfunktionen
*/

void TabLabel::entry(str key, str label)
{
	str special = NULL;
	int width = 0;

	if	(key[0] == '@')
	{
		this.special += label + '\n';
		return;
	}

	if	(this.name && key != ".")
	{
		this.list = paste(",", this.list, key);

		if	(key[0] == ':')
			key = split(key, ":[").next.obj;

		key = paste("=", this.name, key);
	}
	else	key = NULL;

	for (;;)
	{
		if	(label[0] == '[')
		{
			int n = index(label[0], ']');
			width = substr(label, 1);
			label = substr(label, n + 2);
			special += sprintf("\\lineheight{%d}\n", width);

			continue;
		}
		else if	(label[0] == '@')
		{
			special += "\\emptyline\n";
		}
		else if	(label[0] == '*')
		{
			special += "\\boldline\n";
		}
		else if	(label[0] == '-')
		{
			special += "\\boldrule\n";
		}
		else if	(label[0] == '+')
		{
			special += "\\indent ";
		}
		else	break;

		label = substr(label, 1);
	}

	this.tab += { dim(tab), width, special, key, label };
}

TabLabel TabLabel(str name, str axis)
	return { name, axis, NULL, DataBase(Index_t), NULL };

TabLabel TabLabel(str name, str axis, IO io)
{
	str key, label;

	TabLabel x = { name, axis, NULL, DataBase(Index_t), NULL };

	while (getline(io, key, label))
		x.entry(key, label);

	return x;
}

/*	Dummylabel
*/

TabLabel CDummyTabLabel = TabLabel("CDummy", NULL, string !
A	Spalte A
B	Spalte B
C	Spalte C
D	Spalte D
!);

TabLabel LDummyTabLabel = TabLabel("LDummy", NULL, string !
1	Zeile 1
2	Zeile 2
3	Zeile 3
4	Zeile 4
.	.......
n	*Zeile n
!);

#endif	/* _TM_TABLABEL */

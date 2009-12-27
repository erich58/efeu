/*	Label-Tabelle generieren
	(c) 2000 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5
*/

#ifndef	_label_ms
#define	_label_ms	1

#include <var.ms>

struct LabelEntry {
	str name;
	str desc;
};

struct Label {
	str name;
	DataBase db;
};

virtual int dim (Label x)	dim(x.db)

Label Label (str name, ...)
	return { psub(name, va_list), DataBase(str) };

void Label::add (str name, str desc = NULL)
	db += sprintf("{ %#s, %#s }", name, desc);

void Label::tab (bool glob = false)
{
	int n = dim(db);
	IO io = glob ? hdr : src;

	fpsub(io, "\n#define\t$1_dim\t$2\n", name, n);

	if	(n == 0)
	{
		fpsub(io, "#define\t$1\tNULL\n\n");
	}
	else	DeclVec(glob, "Label_t $1_tab", n, "$(db[n]:%s)");
}

void Label::load (IO io)
{
	str a, b; 

	while (getline(io, a, b))
		add(a, b);
}

void Label::range (str nfmt, str lfmt, int first, int last)
	for (int n = first; n <= last; n++)
		add(psub(nfmt), psub(lfmt));

#endif	/* label.ms */

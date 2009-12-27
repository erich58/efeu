/*	Datenstrukturen generieren
	(c) 1998 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

#ifndef	_MS_STRUCT
#define	_MS_STRUCT	1

#include <macro.ms>

/*	Strukturvariablen
*/

struct StructVar {
	str def;	/* Variablendefinition */
	str desc;	/* Beschreibungstext */
};

StructVar StructVar (str def, str desc)
	return { psub(def), psub(desc) }

StructVar StructVar (str type, str name, str desc)
	return { psub(paste(" ", type, name)), psub(desc) }

StructVar StructVar (int size, str name, str desc)
	return { sprintf("unsigned %s : %d", psub(name), size), psub(desc) }

StructVar StructVar (int size, int num)
	StructVar (size, sprintf("_D%d", num), "Ausrichtungsdummy");

void StructVar::typeline (IO io)
{
	fprintf(io, "\t%s;", def)

	if	(desc)	fprintf(io, "\t/* %s */", desc);
	
	io << '\n';
}


/*	Strukturdefinition generieren
*/

void Struct (str Name, str Desc, IO Def, bool flag = true)
{
	int i, n, unused;
	int size = 0;
	int recl = 0;
	int ndummy = 0;

	Name = psub(Name);
	Desc = psub(Desc);
	
	if	(!Desc)	Desc = sprintf("Datenstruktur %s", Name);

	DataBase db = DataBase(StructVar);

	str type, name, desc;

	while (getline(Def, type, name, desc))
	{
		if	(type == "@macro")
			db += StructVar(name, desc);
		else if	(isdigit(type[0]))
			db += StructVar((int) type, name, desc);
		else	db += StructVar(type, name, desc);
	}

	IO io = flag ? hdr : src;
	n = dim(db);

/*	Struktur generieren
*/
	fpsub(io, "\n/*\t$2\n*/\n", Name, Desc);
	io << "\ntypedef struct {\n";

	for (x in db)
		x.typeline(io);

	fpsub(io, "} $1_t;\n\n");

/*	Dokumentation ausgeben
*/
	if	(flag)
	{
		fpsub(info, "@$1:$2");

		for (x in db)
			fprintf(info, "%s\t%s\n", x.def, x.desc)

		info << '\n';
	}
}

#endif	/* _MS_STRUCT */

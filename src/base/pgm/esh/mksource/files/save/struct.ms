/*	Datenstrukturen generieren
	(c) 1998 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

#ifndef	_MS_STRUCT
#define	_MS_STRUCT	1

#include <macro.ms>

struct StructVar {
	str type;	/* Variablentype */
	str name;	/* Variablenname */
	str desc;	/* Beschreibungstext */
};

StructVar StructVar (str type, str name, str desc)
	return { psub(type), psub(name), psub(desc) }

StructVar StructVar (int size, str name, str desc)
	return { "unsigned", sprintf("%s : %d", psub(name), size), psub(desc) }

StructVar StructVar (int size, int num)
	StructVar (size, sprintf("_D%d", num), "Ausrichtungsdummy");

void StructVar::typeline(IO io)
{
	fprintf(io, "\t%s %s;", type, name)

	if	(desc)	fprintf(io, "\t/* %s */", desc);
	
	io << '\n';
}

void StructVar::docline(IO io)
	fprintf(io, "%s %s\t%s\n", type, name, desc)


/*	Sourcefile öffnen und Strukturdefinition laden
*/

void Struct(str Name, str Desc, IO Def, bool flag = true)
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
		if	((int) type)
		{
			db += StructVar((int) type, name, desc);
		}
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
		fpsub(doc, "@$1:$2");

		for (x in db)
			x.docline(doc);

		doc << '\n';
	}
}

#endif	/* _MS_STRUCT */

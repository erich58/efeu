/*	Bitfield - Datenstrukturen generieren
	(c) 1995 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

#ifndef	_MS_BITFIELD
#define	_MS_BITFIELD	1

#include <macro.ms>

struct BitfieldVar {
	int size;	/* Variablenlänge */
	str name;	/* Variablenname */
	str desc;	/* Beschreibungstext */
};

BitfieldVar BitfieldVar (int size, str name, str desc)
	return { size, name, desc }

BitfieldVar BitfieldVar (int size, int num)
	BitfieldVar (size, sprintf("_D%d", num), "Ausrichtungsdummy");

void BitfieldVar::typeline(IO io)
{
	fprintf(io, "\tunsigned %s : %d;", name, size)

	if	(desc)	fprintf(io, "\t/* %s */", desc);
	
	io << '\n';
}


/*	Sourcefile öffnen und Strukturdefinition laden
*/

void Bitfield(str Name, str Desc, IO Def, bool flag = true)
{
	str name, desc;
	int i, n, unused;
	int size = 0;
	int recl = 0;
	int ndummy = 0;

	Name = psub(Name);
	Desc = Desc ? psub(Desc) : "Bitfeldstruktur " + Name;

/*	Bitfeldeinträge generieren
*/
	DataBase db = DataBase(BitfieldVar);

	while (getline(Def, n, name, desc))
	{
		if	(strlen(desc) == 0)	desc = NULL;

		if	(n > 32)	n = 32;

		if	(size + n > 32)
		{
			db += BitfieldVar(32 - size, ++ndummy);
			size = 0; 
			recl += 4;
		}

		db += BitfieldVar(n, name, desc);
		size += n;

		if	(size == 32)
		{
			recl += 4;
			size = 0;
		}
	}

/*	Ungenützte Bytes bestimmen
*/
	if	(size)
	{
		size = 32 - size;
		unused = size / 8;
		size -= 8 * unused;
		recl += 4 - unused;

		if	(size)
			db += BitfieldVar(size, ++ndummy)

		if	(unused)
			db += BitfieldVar(8 * unused, ++ndummy)
	}
	else	unused = 0;

/*	Header ausgeben
*/
	IO io = flag ? hdr : src;
	n = dim(db);

	fpsub(io, "\n/*\t$2\n*/\n", Name, Desc);
	io << "\ntypedef struct {\n";
	io << "#if\tREVBYTEORDER\n";

	for (i = 1; i <= n; i++)
		db[n - i].typeline(io);

	io << "#else\n";

	for (i = 0; i < n; i++)
		db[i].typeline(io);

	io << "#endif\n";

	fpsub(io, "} $1_t;\n\n");
	Macro(io, "$1_recl", NULL, sprintf("%d", recl));
	Macro(io, "$1_size", NULL, "sizeof($1_t)");
	io << "\n";
	Macro(io, "$1_read", "io, data, dim",
		"io_dbread(io, data, $1_recl, $1_size, dim)");
	Macro(io, "$1_write", "io, data, dim",
		"io_dbwrite(io, data, $1_recl, $1_size, dim)");
	Macro(io, "$1_clear", "data, dim",
		"memset(data, 0, (dim) * $1_size)");
	io << "\n";

/*	Dokumentation ausgeben
*/
	fpsub(info, "@$1_t:$2\n");

	for (x in db)
		fprintf(info, "unsigned %s : %d\t%s\n", x.name, x.size, x.desc)

	info << '\n';
}

#endif	/* _MS_BITFIELD */

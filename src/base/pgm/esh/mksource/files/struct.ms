/*
:*:create simple data types
:de:Einfache Datenstrukturen generieren

$Header	<$1>
$Copyright (C) 1998 Erich Frühstück
This file is part of EFEU.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this library; see the file COPYING.Library.
If not, write to the Free Software Foundation, Inc.,
59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
*/

#ifndef	struct_ms
#define	struct_ms	1

#include <macro.ms>
#include <function.ms>

static struct Struct {
	str s_name;
	str s_sname;
	str s_var;
	str s_info;
	str s_func;
	str s_proto;
	str s_bftab;
	str s_mtab;
	str s_ftab;
	str s_setup;
	str s_read;	/* Lesefunktion */
	str s_write;	/* Schreibfunktion */
	str s_clean;	/* Löschfunktion */
	str up_init;
	str up_body;
	str sync_init;
	str sync_body;
	str sync_exit;
	bool private;
	bool efitype;
}

/*
:*:The data type |$1| allows the construction of C structures.
:de:Der Datentype |$1| erlaubt die Konstruktion von C Datenstrukturen.
*/

Struct Struct (str _name, ...)
{
	s_name = psub(_name, va_list);
}

/*
:*:The function |$1| provides include statements for type definitions.
:de:Die Funktion |$1| sorgt für die zur Typedefinition notwendigen
Headerdateien.
*/

void Struct::DataTypeHeader (bool glob = true)
{
	static bool need_object = true;
	static bool need_stdtype = true;

	if	(glob && need_object)
	{
		StdInclude(hdr, "EFEU/object");
		need_object = false;
	}

	if	(need_stdtype)
	{
		StdInclude(src, "EFEU/stdtype");
		need_stdtype = false;
	}
}

static void Struct::use_type (str type)
{
	switch (type)
	{
	case "char":
	case "int":
	case "unsigned":
	case "float":
	case "double":
		break;
	default:
		s_setup += psub("$1_setup();\n", type);
		break;
	}
}

static void Struct::addvar (str tname, str name, str desc)
{
	str p = paste(tname == /[*]$/ ? NULL : " ", tname, name);
	s_info += sprintf("%s\t%s\n", p, desc);
	s_var += sprintf("\t%s;", p);

	if	(desc) s_var += sprintf("\t/* %s */", desc);

	s_var += "\n";
}

static void Struct::bitfield (str name, int size, str desc)
{
	addvar("unsigned", sprintf("%s : %d", name, size), desc);

	s_func += psub(string !
static void $1_$2_get (unsigned *data, void *ptr)
{
	*data = (($1 *) ptr)->$2;
}

static void $1_$2_set (unsigned *data, void *ptr)
{
	(($1 *) ptr)->$2 = *data;
}

!, "$1", name);
	s_bftab += psub("\t{ $2%#s, $3%#s, $1_$2_get, $1_$2_set },\n",
		"$1", name, desc);
}

static virtual void Struct::efivar (str type, str name, str desc)
{
	int vdim = 0;

	type = psub(type);
	desc = psub(desc);
	Type_t t = (Type_t) type;
	addvar(t.cname, name, desc);

	if	(name[0] == '*')	return;

	if	(regexec("\\[", name))
		(name, vdim) = regmatch("(.*)\\[(.*)\\]", name).next;

	s_setup += sprintf("AddTypeMember(&Type_$1, offsetof($1, %s),", name);
	s_setup += sprintf(" %s, %#s, %#s, %#s);\n", vdim, type, name, desc);
}

static void Struct::subvar (str type, str name, str desc)
{
	type = psub(type);
	desc = psub(desc);
	addvar(type, name, desc);
	s_setup += psub("$1_setup();\n", type);
	s_setup += sprintf("AddTypeMember(&Type_$1, offsetof($1, %s),", name);
	s_setup += sprintf(" 0, %#s, %#s, %#s);\n", type, name, desc);
	s_read += psub("size += $1_read(io, &data->$2, 1);\n", type, name);
	s_write += psub("size += $1_write(io, &data->$2, 1);\n", type, name);
	s_clean += psub("CleanData(&Type_$1, &data->$2, mode);\n", type, name);
}

static void Struct::extvar (str type, str def)
{
	str key = xstrcut(type, ",");

	switch (key)
	{
	case "config":
		for (x in split(def, "%s"))
		{
			str name = xstrcut(x, "=");

			switch (name)
			{
			/*
			case "struct":
				s_name = x ? x : "$1";
				break;
			*/
			case "setup":
				s_sname = x ? x : "$1_setup";
				break;
			default:
				fprintf(ioerr, "%s: %#s\n", name, x);
				break;
			}
		}

		break;
	case "macro":
		str name = xstrcut(def, "%s");
		s_var += sprintf("\t%s;\t/* %s */\n", name, def);
		s_info += sprintf("%s\t%s\n", name, def);
		break;
	case "efitype":
		efitype = true;
		s_sname = "$1_setup";
		break;
	case "setup":
		s_sname = "$1_setup";
		break;
	case "strbuf":
		if	(type)
		{
			str name = xstrcut(def, "%s");
			str len = xstrcut(type, ",");
			mkstrbuf(name, len, (int) type);
		}
		else 	message("Externer Stringbuffer $1\n", name);

		break;
	case "str":
		str name = xstrcut(def, "%s");

		if	(type)
		{
			up_body += sprintf("data->%s = nextstr(&%s_ptr);\n",
				name, type);
			sync_body += sprintf("sb_putstr(data->%s, %s_buf);\n",
				name, type);
			sync_body += sprintf("memfree(data->%s);\n", name); 
		}

		efivar("char *", "str", name, def);
		break;
	default:
		message("Unbekanntes Kennwort $1\n", key);
		break;
	}
}

/*
:*:The function |$1| adds structure members to the structur
definition.
:de:Die Funktion |$1| erweitert die Strukturdefinitionen entsprechend
der Einträge in <def>. Eine einzelne Zeile hat die Form
	<type> <name> <desc>
wobei <type> der Datentyp, <name> der Variablenname und <desc> der
Beschreibungstext ist. Eine Zahl anstelle von <type> generiert einen
Bitfeldeintrag der entsprechenden Größe.
Ein . oder das Schlusselwort
|@macro| anstelle von <type> erzeugt einen Eintrag der Form |"|<name>|;"|.
Vergleiche dazu untenstehendes Beispiel.
Falls <type> mit einem Klammeraffen beginnt,
handelt es sich um eine Steueranweisung oder einen Spezialeintrag.
Folgende Anweisungen sind definiert:
\ttitem \@efitype
Es wird ein EFEU-Kompatibler Datentyp eingerichtet. Als Typbezeichner
müssen EFEU-Datentypen angegeben werden.
\end
(Liste unvollständig)
*/

void Struct::addentry (IO in)
{
	str name, desc;
	str key, def;

	while (getline(in, key, def))
	{
		if	(key[0] == '@')
		{
			extvar(substr(key, 1), def);
			continue;
		}

		name = psub(xstrcut(def, "%s"), s_name);
		desc = psub(def, name);

		if	(key == ".")
		{
			s_var += sprintf("\t%s;\t/* %s */\n", name, desc);
			s_info += sprintf("%s\t%s\n", name, desc);
		}
		else if	(isdigit(key[0]))
		{
			bitfield(name, key, desc);
		}
		else if	(key == /^C:/)
		{
			addvar(substr(key, 2), name, desc);
		}
		else if	(name[0] == '*')
		{
			addvar(key, name, desc);
		}
		else if	(efitype)
		{
			efivar(key, name, desc);
		}
		else	addvar(key, name, desc);
	}
}
	
/*
:*:The function |$1| writes the structur declaration into the file
<out>. The argument <post> terminates the declaration.
:de:Die Funktuion |$1| schreibt die Strukturdefionition in die Datei
<out>. Das Argument <post> wird der Strukturdefinition nachgestellt.
Dies erlaubt die Ausgabe der Strukturdefinition direkt in einer
|typedef|-Anweisung oder einer Variablendeklaration.
*/

void Struct::write (IO out, str post = ";\n")
{
	out << "struct";

	if	(s_name)
		out << " " << s_name;

	out << " {\n";
	out << s_var;
	out << "}" << post;
}

/*
:*:The function |$1| crates a |typedef| instruction with
the structure declaration.
:de:Die Funktion |$1| generiert eine |typedef|-Anweisung mit
der Strukturdefinition.
*/

void Struct::type (IO out, str name)
{
	name = psub(name);

	if	(s_name)
	{
		fpsub(out, "\ntypedef struct $1 $2;\n\n", s_name, name);
		this.write(out);
	}
	else
	{
		out << "\ntypedef ";
		this.write(out, paste(name, " ", ";\n\n"));
	}
}

/*
:*:The function |$1| creates a EFI function which is registered on
setting up the data type.
:de:Die Funktion |$1| definiert eine EFI Funktion, die beim
Einrichten des Datentyps regristriert wird.
*/

void Struct::func (str name, str proto, str body)
{
	str fname = "func_$1_" + name;
	proto = psub(proto, "$1", name);
	body = psub(body, "$1", name);

	s_func += "static void " + fname;
	s_func += " (EfiFunc *func, void *rval, void **arg)\n"; 
	s_func += "{\n";

	for (x in split(body, "\n"))
		s_func += "\t" + x + "\n";

	s_func += "}\n\n";
	s_ftab += psub("\t{ 0, NULL, \"" + proto + "\", func_$1_$2 },\n",
		s_name, name);
}

/*
:*:The function |$1| crates a setup function to register the
structure as EFI-Datatype.
:de:Die Funktion |$1| generiert eine Initialisierungsfunktion
zur Registrierung der Struktur als EFI-Datentyp.
*/

void Struct::setup (str fname, bool glob = true)
{
	DataTypeHeader(glob);
	Function f;

	fpsub(src, s_func, s_name);

	str f_read = "NULL";
	str f_write = "NULL";
	str f_clean = "NULL";

	if	(up_init || up_body)
	{
		f = Function("void $1_update", "$1 *data", s_name);
		f.body(up_init + up_body, s_name);
		f.write(false);
		s_read += psub("$$1_update(data);\n");
	}

	s_func = NULL;

	if	(sync_init || sync_body)
	{
		f = Function("void $1_sync", "$1 *data", s_name);
		f.body(sync_init + sync_body + sync_exit, s_name);
		f.line("$1_update(data);", s_name);
		f.write(false);
		func("sync", "void $1::$2 ()", "$1_sync(arg[0]);");
	}

	fpsub(src, s_func, s_name);

	if	(s_read)
	{
		f_read = psub("Type_$1_read", s_name);
		f = Function("size_t " + f_read,
			"const EfiType *type, void *opaque_ptr, IO *io")
		f.line("$1 *data = opaque_ptr;", s_name);
		f.line("size_t size = 0;");
		f.body(s_read, s_name);
		f.line("return size;");
		f.write(false);
	}

	if	(s_write)
	{
		f_write = psub("Type_$1_write", s_name);
		f = Function("size_t " + f_write,
			"const EfiType *type, const void *opaque_data, IO *io")
		f.line("const $1 *data = opaque_data;", s_name);
		f.line("size_t size = 0;");
		f.body(s_write, s_name);
		f.line("return size;");
		f.write(false);
	}

	if	(s_clean)
	{
		f_clean = psub("Type_$1_clean", s_name);
		f = Function("void " + f_clean,
			"const EfiType *type, void *opaque_data, int mode")
		f.line("$1 *data = opaque_data;", s_name);
		f.body(s_clean, s_name);
		f.write(false);
	}

	Define(glob ? hdr : src, string !
$1_recl	0
$1_size	sizeof($1)
!, s_name);

	if	(glob)
	{
		fpsub(hdr, "extern EfiType Type_$1;\n", s_name);
		fpsub(hdr, s_proto, s_name);
	}
	else	src << "static ";

	fpsub(src, "EfiType Type_$1 = COMPLEX_TYPE($1%#s, $1%#s, ", s_name);
	fpsub(src, "sizeof($1), 0,\n\t", s_name);
	fprintf(src, "%s, %s, NULL, NULL, NULL, %s, NULL, TYPE_MALLOC);\n\n",
		f_read, f_write, f_clean);

	f = Function("void " + fname, NULL, s_name);
	f.line("static int setup_done = 0;");
	f.line();
	f.line("if	(setup_done)	return;");
	f.line();
	f.line("setup_done = 1;");
	f.line("AddType(&Type_$1);", s_name);
	f.body(s_setup, s_name);
	f.line("AddMemberFunc(&Type_$1);", s_name);
	f.line("AddListConv(&Type_$1, List2Struct, Struct2List);", s_name);

	if	(s_bftab)
	{
		fprintf(src, "static EfiBFMember %s_bftab[] = {\n", s_name);
		fpsub(src, s_bftab, s_name);
		fprintf(src, "};\n\n");
		f.line("AddEfiBFMember(Type_$1.vtab, \
$1_bftab, tabsize($1_bftab));", s_name);
	}

	if	(s_mtab)
	{
		fprintf(src, "static EfiMember %s_mtab[] = {\n", s_name);
		fpsub(src, s_mtab, s_name);
		fprintf(src, "};\n\n");
		f.line("AddEfiMember(Type_$1.vtab, \
$1_mtab, tabsize($1_mtab));", s_name);
	}

	if	(s_ftab)
	{
		fprintf(src, "static EfiFuncDef %s_ftab[] = {\n", s_name);
		fpsub(src, s_ftab, s_name);
		fprintf(src, "};\n\n");
		f.line("AddFuncDef($1_ftab, tabsize($1_ftab));", s_name);
	}
	
	f.write(true);
}

/*
:*:The function |$1| writes the structure description to the info output.
:de:Die Funktion |$1| schreibt eine Strukturbeschreibung
in die Info-Ausgabedatei.
*/

void Struct::mkinfo (str name, str desc)
{
	fprintf(info, "@%s:%s\n", name, desc);
	info << s_info;
	info << '\n';
}

static void Struct::mkstrbuf (str name, str size, int bsize)
{
	addvar("char *", name, "Textbuffer");
	addvar("size_t", name + "_size", "Aktuelle Größe des Textbuffers");

	s_func += psub(string !
static void $$1_$1_clean ($$1 *data)
{
	lfree(data->$1);
	data->$1_size = 0;
	data->$1 = NULL;
}

static void $$1_$1_realloc ($$1 *data)
{
	if	(data->$1_size < data->$2)
	{
		lfree(data->$1);
		data->$1_size = sizealign(data->$2, $3);
		data->$1 = lmalloc(data->$1_size * sizeof(char));
	}
}

!, name, size, bsize);

	s_read += psub("$$1_$1_realloc(data);\n", name);
	s_read += psub("size += io_read(io, data->$1, data->$2);\n",
		name, size);
	s_write += psub("size += io_write(io, data->$1, data->$2);\n",
		name, size);
	s_clean += psub("$$1_$1_clean(data);\n", name);
	up_init += psub("char *$1_ptr = data->$1;\n", name);
	sync_init += psub("StrBuf *$1_buf = sb_create(0);\n", name);
	sync_exit += psub(string !
data->$2 = sb_size($1_buf);
$$1_$1_realloc(data);
memcpy(data->$1, $1_buf->data, data->$2);
rd_deref($1_buf);
!, name, size);
}

/*
:*:The function |$1| creates string variables which use a common
character buffer for storing data.
:de:Die Funktion |$1| generiert Stringvariablen mit einem gemeinsamen
Zeichenbuffer.
*/

void Struct::txtvar (str name, str size, int bsize, IO def)
{
	str var, desc;

	mkstrbuf(name, size, bsize);

	while (getline(def, var, desc))
	{
		efivar("str", var, desc);
		up_body += sprintf("data->%s = nextstr(&%s_ptr);\n",
			var, name);
		sync_body += sprintf("sb_putstr(data->%s, %s_buf);\n",
			var, name);
		sync_body += sprintf("memfree(data->%s);\n", var); 
	}
}


/*
:*:The function |$1| creates a local structure type (output to source
file).
:de:Die Funktion |$1| generiert einen lokalen Strukturdatentyp
(Ausgabe ins Sourcefile).
*/

void LocalType (str name, str desc, IO def, ...)
{
	name = psub(name, va_list);

	if	(desc)
		fprintf(top, "\n/*\t%s\n*/\n", psub(desc, name));

	fpsub(top, "\ntypedef struct $1 $1;\n\n", name);

	Struct st = Struct(name);
	st.addentry(psubfilter(def, va_list));
	st.write(top);

	if	(st.s_sname)
		st.setup(st.s_sname, false);
}

/*
:*:The function |$1| creates a global structure type (output to hdr). If <desc>
is not NULL, the structure description is written to the info output.
:de:Die Funktion |$1| generiert einen globalen Strukturdatentyp
(Ausgabe ins Headerfile).
Falls <desc> verschieden von 0 ist, wird eine Strukturbeschreibung
in die Info-Ausgabedatei geschrieben.
*/

void GlobalType (str name, str desc, IO def, ...)
{
	name = psub(name, va_list);
	desc = psub(desc, name);

	if	(desc)
		fprintf(hdr, "\n/*\t%s\n*/\n", desc);

	fpsub(hdr, "\ntypedef struct $1 $1;\n\n", name);

	Struct st = Struct(name);
	st.addentry(psubfilter(def, va_list));
	st.write(hdr);

	if	(desc)
		st.mkinfo(name, desc);

	if	(st.s_sname)
		st.setup(st.s_sname, true);
}

/*
$Example
\ms_example
config hdr info

#include <struct.ms>

StdInclude(hdr, "EFEU/refdata");

GlobalType("Data", "example data type with reference counter", string !
@macro	REFVAR	reference variables (-> refdata(3))
12	b1	bitfield
10	b2	bitfield
int	i1[4]	integer vector
double	d	floting point number
unsigned\ char	c	unsigned character variable
!);
\end
*/

#endif	/* struct.ms */

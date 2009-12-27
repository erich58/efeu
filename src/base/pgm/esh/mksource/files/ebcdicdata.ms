/*	EBCDIC-Rohdatenfelder
	(c) 1998 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

#ifndef	_MS_EBCDICDATA
#define	_MS_EBCDICDATA	1

#include <macro.ms>
#include <function.ms>

void EBCDICMacro(str sname, str label, int recl, IO def, bool glob = true)
{
	sname = psub(sname);
	label = label ? psub(label) : "Datenmakro " + sname;

	fprintf(info, "@%s:%s\n", sname, label);

	str name, key, desc, func;
	int pos, len;
	IO io = glob ? hdr : src;

	while (getline(def, name, key, pos, len, desc))
	{
		func = key;

		if	(strlen(key) == 1)
			func += "val";

		fprintf(io, "#define\t%s_%s(x)\t", sname, name);
		fprintf(io, "db_%s(x, %d, %d)\n", func, pos, len);
		fprintf(info, "%s\t%s\t%d\t%d\t%s\n", name, key, pos, len, desc)
	}

	fpsub(io, "\ntypedef uchar_t $1_t;\n", sname);
	fprintf(io, "\n#define\t%s_recl\t%s\n", sname, recl);
	fprintf(io, "#define\t%s_size\t%s\n\n", sname, recl);
	info << '\n';
}

void EBCDICData(str sname, str label, int recl, IO def, bool glob = true)
{
	sname = psub(sname);
	label = label ? psub(label) : "EBCDIC-Datentype " + sname;

	IO io = glob ? hdr : src;

	fprintf(io, "\n/*\t%s\n*/\n\n", label);

	EBCDICMacro(sname, label, recl, def);

	setreg(1, sname);
	Macro(io, "$1_read", "io, data, dim",
		"io_read(io, data, (dim) * $1_recl)");
	Macro(io, "$1_write", "io, data, dim",
		"io_write(io, data, (dim) * $1_recl)");
	Macro(io, "$1_clear", "data, dim",
		"memset(data, 0, (dim) * $1_size)");
}

void EBCDICPrint (str type, str xdecl, IO def, bool nl = true, bool glob = true)
{
	type = psub(type);
	Function f = Function("int $1_print",
		paste(", ", "io_t *out, $1_t *x, size_t dim", xdecl), type);
	f.line("register int n;");
	f.line();
	f.line("for (n = 0; dim-- > 0; x += $1_recl)");
	f.beg();
	bool flag = f.pfbody("out", def);

	if	(nl)
	{
		if	(flag)	f.newpar();

		f.line("io_putc('\\n', out);");
		f.line("n++;");
	}

	f.end();
	f.line();
	f.line("return n;");
	f.write(glob);
}


void VRData(str sname, str label, bool glob = true)
{
	sname = psub(sname);
	label = label ? psub(label) : "VR-Datentype " + sname;

	IO io = glob ? hdr : src;

	fprintf(io, "\n/*\t%s\n*/\n\n", label);
	fpsub(io, "\ntypedef VRData_t $1_t;\n", sname);
	Define(io, string !
$1_recl	VRData_recl
$1_size	VRData_size
$1_read	VRData_read
$1_write	VRData_write
$1_clear	VRData_clear
!);
}

#endif	/* _MS_EBCDICDATA */

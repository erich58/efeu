/*	Synthesis-Parameterdateien
	(c) 2000 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5
*/

#ifndef	_synpar_ms
#define	_synpar_ms	1

#include <syntools.ms>

void SynPar (str name, str label, str def,
	str sfunc, str pfunc, bool glob = true)
{
	name = psub(name);
	label = psub(label);
	DBData(name, label, paste("\n", "ext . SYNPAR_VAR", def), glob);

	src << "\n";

	if	(glob)
		fpsub(hdr, "\nextern SynParType_t $1_Type;\n", name);
	else	src << "static ";

	str tname = name + "_Type";
	psub(NULL, tname, name);

	LocalFunction("void $1_clear", "void *par",
		"$2_clear(par, 1);");
	LocalFunction("void $1_read", "void *par, io_t *io",
		"$2_read(io, par, 1);");
	LocalFunction("void $1_write", "void *par, io_t *io",
		"$2_write(io, par, 1);");
	LocalFunction("void $1_scan", "$2_t *par, io_t *io", sfunc);
	LocalFunction("void $1_print", "$2_t *par, io_t *io", pfunc);

	fpsub(src, "SynParType_t $1 = {\n\t");
	fprintf(src, "%#s, %#s,\n\t", name, label);
	fpsub(src, "$2_size, $1_clear,\n\t");
	fpsub(src, "$1_read, $1_write,\n\t");
	fpsub(src, "(SynParIO_t) $1_scan, (SynParIO_t) $1_print,\n\t");
	src << "};\n\n";
}

#endif	/* synpar.ms */

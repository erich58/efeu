/*	Synthesis-Datenbankkonverter
	(c) 2000 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5
*/

#ifndef	_synsetup_ms
#define	_synsetup_ms	1

#include <syntools.ms>
#include <syncount.ms>
#include <syndb.ms>
#include <synkonv.ms>
#include <synjoin.ms>

struct SynSetup {
	Function f;
};

SynSetup SynSetup (str name)
{
	Function f = Function("void SynSetup_$1", NULL, name);
	f.line("static int setup_done = 0;");
	f.line();
	f.line("if\t(setup_done)\treturn;");
	f.line();
	return (SynSetup) { f };
}

void SynSetup::write (bool glob = true)
{
	f.line("setup_done = 1;");
	f.write(glob);
}

void SynSetup::setup (...)
	for (x in va_list)
		f.fline("SynSetup_%s();", psub(x));

void SynSetup::db (str name)
	f.fline("AddSynDBType(&%s_DB);", psub(name));

void SynSetup::line (str fmt, ...)
	f.line(fmt, va_list);

void SynSetup::par (str name)
	f.fline("AddSynParType(&%s_Type);", psub(name));

void SynSetup::add_konv (str db1, str db2)
{
	f.line("AddSynDBKonv(&$1);", psub(paste("_to_", db1, db2)));
	fpsub(src, "extern SynDBKonvDef_t $1;\n");
}

void SynSetup::konv (str d1, str d2, str body,
	str p = NULL, str v = NULL, str desc = NULL)
	f.line("AddSynDBKonv(&$1);",
		SynKonvDef(d1, d2, body, p, v, desc, false));

void SynSetup::add_join (str db0, str db1, str db2)
{
	f.line("AddSynDBJoin(&$1);", psub(sprintf("Join_%s_%s", db1, db2)));
	fpsub(src, "extern SynDBJoinDef_t $1;\n");
}

void SynSetup::join (str r, str a, str b, str d,
	str p = NULL, str v = NULL, str desc = NULL)
	f.line("AddSynDBJoin(&$1);",
		SynJoinDef(r, a, b, d, p, v, desc, false));

#endif	/* synsetup.ms */

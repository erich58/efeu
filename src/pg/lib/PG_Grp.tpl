config hdr src

#include <TypeDef.ms>

StdInclude(hdr, "DB/PG");
StdInclude(top, "DB/PG_Grp");

hdr << "#if HAS_PQ\n";
top << "#if HAS_PQ\n";

StdInclude(top, "ctype");

str NAME = BaseName;

TypeDef def = TypeDef(NAME,
	"Gruppendefinition für die Würfelkonstruktion", string !
str name	Gruppenname
int mode	Methode für Bezeichner
str query	Abfragekommando
str key		Schlüssel
str code	Kode
str label	Format für Label
str head	Format für Kopf
str delim	Trennzeichen
!);

def.setup(true);

/*	Parameterdefinitionen
*/

str ptab = psub(string !
static struct {
	const char *name;
	void (*func) ($1 *grp, char *arg);
} ptab[] = {
!, NAME);

void ParFunc (str name, str expr)
{
	Function f = Function("void $1_par_$2",
		"$1 *grp, char *arg", NAME, name);
	f.body(expr);
	f.write(false);
	ptab += psub("\t{ $[2;%#s], $1_par_$2 },\n", NAME, name);
}

ParFunc("label", "grp->label = arg;");
ParFunc("head", "grp->head = arg;");
ParFunc("query", "grp->mode = 2; grp->query = arg;");
ParFunc("fetch", "grp->mode = 1; grp->query = arg;");
ParFunc("key", "grp->key = arg;");
ParFunc("code", "grp->code = arg;");
ParFunc("delim", "grp->delim = arg;");

src << ptab + "};\n";

/*
Label - Funktionen
*/

StdFunc("void $1_std", "PG *pg, EfiStruct *st, PG_Grp *grp", string !
VarTabEntry *var;
size_t n;

var = st->type->vtab->tab.data;
st->desc = mstrcpy(grp->head);

for (n = st->type->vtab->tab.used; n-- > 0; var++)
	var->desc = mpsubarg(grp->label, "ns", var->name);
!, NAME);

LocalFunction("char *$1_mklabel", "PGresult *res, $1 *grp", string !
if	(res && PQresultStatus(res) == PGRES_TUPLES_OK)
{
	int nf = PQnfields(res);
	char **arg = lmalloc((nf + 1) * sizeof arg[0]);
	StrBuf *sb = sb_acquire();
	char *p;
	int i;

	for (i = 0; i < nf; i++)
	{
		if	(i)
			sb_puts(grp->delim, sb);

		p = PQfname(res, i);
		sb_puts(p, sb);
		arg[i + 1] = p;
	}

	arg[0] = sb_nul(sb);

	if	(grp->head)
	{
		p = mpsubvec(grp->head, nf + 1, arg);
	}
	else	p = mstrcpy(arg[0]);

	lfree(arg);
	sb_release(sb);
	return p;
}

return mpsub(grp->head, NULL);
!, NAME);

LocalFunction("char *$1_mkdesc",
	"PGresult *res, int n, $1 *grp, const char *fmt", string !
if	(res && PQresultStatus(res) == PGRES_TUPLES_OK)
{
	int nf = PQnfields(res);
	char **arg = lmalloc((nf + 1) * sizeof arg[0]);
	StrBuf *sb = sb_acquire();
	char *p;
	int i;

	for (i = 0; i < nf; i++)
	{
		if	(i)	sb_puts(grp->delim, sb);

		p = PQgetvalue(res, n, i);
		sb_puts(p, sb);
		arg[i + 1] = p;
	}

	arg[0] = sb_nul(sb);

	if	(fmt)
	{
		p = mpsubvec(fmt, nf + 1, arg);
	}
	else	p = mstrcpy(arg[0]);
	
	lfree(arg);
	sb_release(sb);
	return p;
}

return mpsub(grp->label, NULL);
!, NAME);

StdFunc("void $1_fetch", "PG *pg, EfiStruct *st, PG_Grp *grp", string !
VarTabEntry *var;
size_t n;

st->desc = mstrcpy(grp->head);
var = st->type->vtab->tab.data;

for (n = st->type->vtab->tab.used; n-- > 0; var++)
{
	char *cmd = mpsubarg(grp->query, "ns", var->name);
	int stat = PG_query(pg, cmd);
	memfree(cmd);

	if	(!stat)	continue;

	if	(!st->desc)
		st->desc = $1_mklabel(pg->res, grp);

	var->desc = $1_mkdesc(pg->res, 0, grp, grp->label);

	if	(grp->code)
	{
		memfree((char *) var->name);
		var->name = $1_mkdesc(pg->res, 0, grp, grp->code);
	}
}

if	(grp->code)
	VarTab_qsort(st->type->vtab);
!, NAME);

StdFunc("void $1_query", "PG *pg, EfiStruct *st, PG_Grp *grp", string !
int i, k, n;

if	(!PG_query(pg, grp->query))
	return;

st->desc = $1_mklabel(pg->res, grp);

n = PQnfields(pg->res);

for (k = 0; k < n; k++)
{
	if	(mstrcmp(grp->key, PQfname(pg->res, k)) == 0)
		break;
}

if	(k >= n)	return;

n = PQntuples(pg->res);

for (i = 0; i < n; i++)
{
	int c = i + 1;
	char *label = mstrcpy(PQgetvalue(pg->res, i, k));
	VarTab_xadd(st->type->vtab, label,
		$1_mkdesc(pg->res, i, grp, grp->label),
		ConstObj(st->type, &c));
}

PG_clear(pg);
!, NAME);

/*
Gruppendefinitionen parsen
*/

StdFunc("void $1_parse", "EfiVec *vec, const char *def", string !
$1 *grp;
StrBuf *buf;
char *cmd;
char *arg;
int i;
int protect;
int arg_pos;

EfiVec_resize(vec, 0);

if	(!def)
	return;

buf = sb_acquire();

for (;;)
{
	while (isspace(*def))
		def++;

	if	(*def == 0)	
		break;

	grp = vb_next(&vec->buf);
	cmd = NULL;

	for (i = 1; def[i]; i++)
		if (isspace(def[i])) break;

	grp->name = mstrncpy(def, i);
	grp->delim = ";";
	def += i;

	while (*def)
	{
		while (*def == ' ' || *def == '\t')
			def++;

		if	(*def == '\n')
		{
			def++;

			if	(*def != ' ' && *def != '\t')
				break;

			def++;
			continue;
		}

		if	(*def == '\\')
		{
			def++;

			if	(*def == '\n')
			{
				def++;
				continue;
			}
		}

		sb_begin(buf);

		while (*def != '=' && !isspace(*def))
		{
			sb_putc(*def, buf);
			def++;
		}

		cmd = sb_nul(buf);
		arg = NULL;

		if	(*def == '=')
		{
			sb_putc(0, buf);
			arg_pos = sb_getpos(buf);
			def++;

			for (protect = 0; *def; def++)
			{
				if	(*def == '"')
				{
					protect = !protect;
				}
				else if	(*def == '\\' && def[1])
				{
					def++;

					switch (*def)
					{
					case '"':
					case '\\':
					case ' ':
						sb_putc(*def, buf);
						break;
					case '\n':
						sb_putc('\n', buf);
						break;
					case '\t':
						sb_putc('\t', buf);
						break;
					default:
						sb_putc('\\', buf);
						sb_putc(*def, buf);
						break;
					}
				}
				else if	(!protect && isspace(*def))
				{
					break;
				}
				else	sb_putc(*def, buf);
			}

			cmd = sb_nul(buf);
			arg = cmd + arg_pos;
		}

		for (i = 0; i < tabsize(ptab); i++)
		{
			if	(mstrcmp(cmd, ptab[i].name) == 0)
			{
				ptab[i].func(grp, mstrcpy(arg));
				break;
			}
		}
	}
}

sb_release(buf);
!, NAME);

hdr << "#endif\t/* HAS_PQ */\n";
src << "#endif\t/* HAS_PQ */\n";

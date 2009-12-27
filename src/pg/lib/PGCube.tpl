config hdr src

#include <TypeDef.ms>

StdInclude(hdr, "DB/PG");
StdInclude(hdr, "DB/PG_Grp");
StdInclude(top, "DB/PGCube");

hdr << "#if HAS_PQ\n";
top << "#if HAS_PQ\n";

StdInclude(top, "EFEU/calendar");
StdInclude(top, "ctype");

str NAME = BaseName;
str PAR = "PGCube_Par";

TypeDef cpar = TypeDef(PAR, "Konvertierungsparameter", string !
@config local
unsigned	offset	Datenoffset
str		name	Name
Type_t		type	Datentyp
C:PG_Grp	*grp	Gruppenpointer
C:void "(*conv)(EfiType *type, void *data, const char *def)"
!);

cpar.setup(false);

TypeDef def = TypeDef(NAME,
	"Datenstruktur für die Würfelkonstruktion", string !
@base
P:PG		pg	Schnittstellenpointer
X:PG_Grp[]	grp	Gruppenvektor
X:$(PAR)[]	par	Konvertierungsparameter
str		cname	Cursorname
Type_t		type	Datentyp
Object		obj	Datenobjekt
EDB		edb	EDB-Datenschnittstelle
int		bsize	Blockgröße zur Datenabfrage
!);

def.use(PAR);
def.use("PG");

void ConvFunc (str name, str expr)
	StdFunc("static void conv_$1",
		"EfiType *type, void *data, const char *def", expr, name);

ConvFunc("int", "Val_int(data) = strtol(def, NULL, 10);");
ConvFunc("date", "Val_Date(data) = str2Calendar(def, NULL, 0);");
ConvFunc("str", "Val_str(data) = mstrcpy(def);");
ConvFunc("any", "Obj2Data(strterm(def), type, data);");
ConvFunc("enum", string !
VarTabEntry *var = VarTab_get(type->vtab, def);

if	(var)
{
	Val_int(data) = Val_int(var->obj->data);
}
else
{
	Val_int(data) = type->vtab->tab.used + 1;
	VarTab_xadd(type->vtab, mstrcpy(def), NULL, ConstObj(type, data));
}
!);

StdFunc("void $1_mktype", "$1 *handle", string !
PG_Grp *grp;
$(PAR) *par;
EfiStruct *var, **ptr;
PGresult *res;
char *cmd;
void *tinfo;
int i, j, nf, ng;

if	(!handle->cname)	return;

if	(!handle->grp)
	handle->grp = NewEfiVec(&Type_PG_Grp, NULL, 0);

if	(!handle->par)
	handle->par = NewEfiVec(&Type_$(PAR), NULL, 0);

tinfo = PGType_create(handle->pg);
cmd = msprintf("fetch 0 in %s", handle->cname);

if	(!PG_query(handle->pg, cmd))
	return;

memfree(cmd);
res = handle->pg->res;
nf = PQnfields(res);
grp = handle->grp->buf.data;
ng = handle->grp->buf.used;
EfiVec_resize(handle->par, nf);
par = handle->par->buf.data;
var = NULL;
ptr = &var;

for (i = 0; i < nf; i++)
{
	par[i].name = mstrcpy(PQfname(res, i));
	char *tname = PGType_name(tinfo, PQftype(res, i));

	if	(mstrcmp(tname, "int4") == 0)
	{
		par[i].type = &Type_int;
		par[i].conv = conv_int;
	}
	else if	(mstrcmp(tname, "int8") == 0)
	{
		par[i].type = &Type_int64;
		par[i].conv = conv_any;
	}
	else if	(mstrcmp(tname, "date") == 0)
	{
		par[i].type = &Type_Date;
		par[i].conv = conv_date;
	}
	else if	(strncmp(tname, "float", 5) == 0)
	{
		par[i].type = &Type_double;
		par[i].conv = conv_any;
	}
	else if	(mstrcmp(tname, "text") == 0)
	{
		par[i].type = &Type_str;
		par[i].conv = conv_str;
	}
	else
	{
		par[i].type = NULL;
		par[i].conv = conv_enum;
	}

	for (j = 0; j < ng; j++)
	{
		if	(mstrcmp(par[i].name, grp[j].name) == 0)
		{
			par[i].conv = conv_enum;
			par[i].type = NULL;
			par[i].grp = grp + j;
			break;
		}
	}

	if	(!par[i].type)
	{
		EfiType *type = NewEnumType(NULL, 4);
		type->defval = memalloc(type->size);
		memset(type->defval, 0, type->size);
		par[i].type = type;
	}

	*ptr = NewEfiStruct(par[i].type, par[i].name, 0);
	ptr = &(*ptr)->next;
}

handle->type = MakeStruct(NULL, NULL, var);
handle->obj = LvalObj(NULL, handle->type);
PG_clear(handle->pg);
PGType_clean(tinfo);

for (i = 0, var = handle->type->list; i < nf; i++)
{
	par[i].offset = var->offset;

	if	(par[i].grp && par[i].grp->mode == 2)
		PG_Grp_query(handle->pg, var, par[i].grp);

	var = var->next;
}
!, NAME);

LocalFunction("int read_binary", "EfiType *type, void *data, void *par",
	string !
if	(io_peek(par) == EOF)
	return 0;

return ReadData(type, data, par) != 0;
!);

StdFunc("void $1_create", "$1 *handle, const char *name", string !
IO *tmp;
char *dname;
char *mname;
char *cmd;
char *ptr;
$(PAR) *par;
EfiStruct *st;
int i, j, nf, nt;

if	(!handle->obj)	return;

handle->edb = edb_alloc(rd_refer(handle->obj), NULL);
dname = name ? msprintf("%s.data.gz", name) : NULL;
mname = name ? msprintf("%s.edb", name) : NULL;
tmp = dname ? io_fileopen(dname, "wz") : io_bigbuf(0x10000, NULL);

cmd = msprintf("fetch %d in %s", 500, handle->cname);
ptr = handle->obj->data;
par = handle->par->buf.data;

while (PG_query(handle->pg, cmd))
{
	nt = PQntuples(handle->pg->res);
	nf = PQnfields(handle->pg->res);

	if	(nt == 0)
		break;

	if	(nf != handle->par->buf.used)
		break;

	for (i = 0; i < nt; i++)
	{
		for (j = 0; j < nf; j++)
		{
			par[j].conv(par[j].type, ptr + par[j].offset,
				PQgetvalue(handle->pg->res, i, j));
		}

		WriteData(handle->obj->type, ptr, tmp);
	}

	PG_clear(handle->pg);
}

nf = handle->par->buf.used;

for (i = 0, st = handle->type->list; i < nf; i++)
{
	if	(par[i].grp)
	{
		switch (par[i].grp->mode)
		{
		case 0:
			PG_Grp_std(handle->pg, st, par[i].grp);
			break;
		case 1:
			PG_Grp_fetch(handle->pg, st, par[i].grp);
			break;
		}
	}

	st = st->next;
}

if	(mname)
{
	IO *meta = io_fileopen(mname, "w");
	edb_head(handle->edb, meta, 2);
	io_printf(meta, "@import binary\n%s\n", dname);
	io_close(meta);
}

if	(dname)
{
	io_close(tmp);
	tmp = io_fileopen(dname, "rz");
}
else	io_rewind(tmp);

edb_input(handle->edb, read_binary, tmp);
memfree(cmd);
memfree(dname);
memfree(mname);
!, NAME);

def.func("create", "void $1::create(str name = NULL)", string !
$1_mktype(arg[0]);
$1_create(arg[0], Val_str(arg[1]));
!);

def.func("close", "void $1::close()", string !
$1 *handle = arg[0];
CleanData(&Type_$1, handle, 0);
!);

def.func("group", "void $1::group(str def)", string !
$1 *handle = arg[0];
PG_Grp_parse(handle->grp, Val_str(arg[1]));
!);

def.func("cursor", "void $1::cursor(str def, str name = \\\"tmpcursor\\\")",
	string !
$1 *handle = arg[0];
char *cmd;
memfree(handle->cname);
handle->cname = mstrcpy(Val_str(arg[2]));
cmd = msprintf("declare %s cursor for %s", handle->cname, Val_str(arg[1]));
PG_serialize(handle->pg);
PG_command(handle->pg, cmd);
memfree(cmd);
!);

def.setup(true);
def = NULL;

hdr << "#endif\t/* HAS_PQ */\n";
src << "#endif\t/* HAS_PQ */\n";

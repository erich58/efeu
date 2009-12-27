/*
Objektdefinitionen

$Header <EFEU/$1>

$Copyright (C) 1994 Erich Frühstück
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

#ifndef	EFEU_object_h
#define	EFEU_object_h	1

#include <EFEU/efmain.h>
#include <EFEU/efio.h>
#include <EFEU/vecbuf.h>
#include <EFEU/ftools.h>
#include <EFEU/nkt.h>

/*	Typedefinitionen für Objekte
*/

typedef struct EfiObjStruct EfiObj;
typedef struct EfiTypeStruct EfiType;
typedef struct EfiFuncStruct EfiFunc;
typedef struct EfiLvalStruct EfiLval;


/*	Vektoren
*/

typedef struct {
	EfiType *type;	/* Variablentype */
	void *data;	/* Datenpointer */
	size_t dim;	/* Vektordimension */
} EfiVec;


/*	Variablen
*/

typedef struct EfiVarStruct EfiVar;

struct EfiVarStruct {
	char *name;		/* Variablenname */
	EfiType *type;		/* Variablentype */
	void *data;		/* Datenpointer */
	size_t dim;		/* Variablendimension */
	size_t offset;		/* Datenoffset */
	EfiObj *(*member) (const EfiVar *member, const EfiObj *obj);
	void *par;		/* Strukturparameter */
	EfiVar *next;		/* Nächster Eintrag */
	char *desc;		/* Beschreibungstext */
	void (*clean) (void *ptr);	/* Aufräumfunktion */
};

EfiObj *Var2Obj (EfiVar *var, const EfiObj *obj);


/*	Variablendefinition
*/

typedef struct {
	char *name;		/* Variablenname */
	EfiType *type;		/* Variablentype */
	void *data;		/* Datenpointer */
	char *desc;		/* Beschreibungstext */
} EfiVarDef;

/*	Variablentabellen
*/

typedef struct {
	const char *name;
	char *desc;
	EfiType *type;
	EfiObj *obj;
	EfiObj *(*get) (const EfiObj *base, void *data);
	void (*clean) (void *data);
	void *data;
} VarTabEntry;

typedef struct {
	REFVAR;
	char *name;
	VecBuf tab;
} EfiVarTab;

extern EfiVarTab *GlobalVar;
extern EfiVarTab *LocalVar;
extern EfiVarTab *ContextVar;
extern EfiObj *LocalObj;
extern EfiObj *ContextObj;

EfiVarTab *VarTab (const char *name, size_t dim);
EfiVarTab *CurrentVarTab (EfiVarTab *tab);
int ShowVarTab (IO *io, const char *pfx, EfiVarTab *vtab);

VarTabEntry *VarTab_get (EfiVarTab *tab, const char *name);
void VarTab_add (EfiVarTab *tab, VarTabEntry *entry);
void VarTab_xadd (EfiVarTab *tab, char *name, char *desc, EfiObj *obj);
void VarTab_del (EfiVarTab *tab, const char *name);

#define	RefVarTab(tab)	rd_refer(tab)
#define	DelVarTab(tab)	rd_deref(tab)

typedef struct {
	const char *name;
	EfiType *type;
	EfiObj *(*get) (const EfiObj *base, void *data);
	void *par;
	const char *desc;
} EfiMember;

void AddEfiMember (EfiVarTab *tab, EfiMember *def, size_t dim);


/*	Virtuelle Funktionen
*/

typedef struct {
	REFVAR;
	VecBuf tab;
} EfiVirFunc;

EfiVirFunc *VirFunc (EfiFunc *func);
int IsVirFunc (void *data);


/*	Objekttype
*/

struct EfiTypeStruct {
	char *name;	/* Typename */
	size_t size;	/* Datenlänge */
	size_t recl;	/* Satzlänge */
	size_t (*read) (const EfiType *type, void *data, IO *io);
	size_t (*write) (const EfiType *type, const void *data, IO *io);
	EfiType *base;	/* Basisobjekt (Vektor, Vererbung) */
	EfiObj *(*eval) (const EfiType *type, const void *src);
	void (*clean) (const EfiType *type, void *tg);
	void (*copy) (const EfiType *type, void *tg, const void *src);
	size_t dim;	/* Vektordimension */
	EfiVar *list;	/* Strukturelemente */
	EfiVarTab *vtab;	/* Variablentabelle */
	EfiFunc *fclean; /* Destruktor, nicht überladbar */
	EfiFunc *fcopy;	/* Kopierfunktion, nicht überladbar */
	VecBuf konv;	/* Konvertierungen */
	EfiVirFunc *create;	/* Konstruktor, überladbar */
	void *defval;	/* Vorgabewert */
};


/*	Zuweisungsobjekte
*/

#define	LVAL_VAR \
	EfiObj *(*alloc) (EfiType *type, va_list list); \
	void (*free) (EfiObj *obj); \
	void (*update) (EfiObj *obj); \
	void (*sync) (EfiObj *obj); \
	char *(*ident) (const EfiObj *obj)

#define	LVAL_DATA(alloc,free,update,sync,ident) \
	alloc, free, update, sync, ident

struct EfiLvalStruct {
	LVAL_VAR;
};

extern EfiLval Lval_ptr;
extern EfiLval Lval_ref;
extern EfiLval Lval_obj;

/*
Objektvariablen
*/

#define	OBJECT_VAR	REFVAR; EfiType *type; EfiLval *lval; void *data

struct EfiObjStruct {
	OBJECT_VAR;
};

EfiObj *Obj_alloc (size_t size);
void Obj_free (EfiObj *obj, size_t size);
void Obj_stat (const char *prompt);
void Obj_check (const EfiObj *obj);
char *Obj_ident (const EfiObj *obj);

/*	Neue Datentypen
*/

EfiType *NewType (char *name);

typedef struct {
	char *name;
	int val;
} EnumTypeDef;

EfiType *NewEnumType (const char *name, EnumTypeDef *def, size_t dim);
void AddEnumKey (EfiType *type, char *name, int val);
char *EnumKeyLabel (const EfiType *type, int val, int flag);
int EnumKeyCode (const EfiType *type, const char *key);


/*	Makro zur Typedefinition
*/

void AddType (EfiType *type);
EfiType *GetType (const char *name);
int IsTypeClass (const EfiType *type, const EfiType *base);


/*	Initialisierungsfunktionen
*/

EfiVar *GetStruct (IO *io, int delim);
EfiType *FindStruct (EfiVar *list, size_t size);
EfiType *MakeStruct (char *name, EfiVar *base, EfiVar *list);

EfiObj *LvalObj (EfiLval *lval, EfiType *type, ...);
EfiObj *NewObj (EfiType *type, void *data);
EfiObj *ConstObj (EfiType *type, const void *data);
EfiObj *RefObj (const EfiObj *obj);
void DeleteObj (EfiObj *obj);
void UnrefObj (EfiObj *obj);
void SyncLval (EfiObj *obj);
EfiObj *AssignTerm (const char *name, EfiObj *left, EfiObj *right);
EfiObj *AssignObj (EfiObj *left, EfiObj *right);

EfiObj *EvalObj (EfiObj *obj, EfiType *type);
void UnrefEval (EfiObj *obj);

int TypeComp (const EfiType *def1, const EfiType *def2);

void CleanData (const EfiType *type, void *tg);
void CopyData (const EfiType *type, void *tg, const void *src);
size_t ReadData (const EfiType *type, void *data, IO *io);
size_t WriteData (const EfiType *type, const void *data, IO *io);
void AssignData (const EfiType *type, void *tg, const void *src);

void CleanVecData (const EfiType *type, size_t dim, void *tg);
void CopyVecData (const EfiType *type, size_t dim, void *tg, const void *src);
size_t ReadVecData (const EfiType *type, size_t dim, void *data, IO *io);
size_t WriteVecData (const EfiType *type, size_t dim, const void *data, IO *io);
void AssignVecData (const EfiType *type, size_t dim, void *tg, const void *src);

void Obj2Data (EfiObj *obj, EfiType *type, void *ptr);

int ShowType (IO *io, const EfiType *type);
void ListType (IO *io, const EfiType *type);
void TypeTree (IO *io, const EfiType *type);

extern EfiType Type_void;	/* Leerer Type */


/*	Ganzzahlobjekte
*/

extern unsigned char Buf_char;	/* Buffer für Zeichenkonstante */
extern char Buf_byte;		/* Buffer für Byteswert */
extern short Buf_short;		/* Buffer für kurze Ganzzahlkonstante */
extern int Buf_int;		/* Buffer für normale Ganzzahlkonstante */
extern long Buf_long;		/* Buffer für lange Ganzzahlkonstante */
extern unsigned Buf_uint;	/* Buffer für Vorzeichenfreie Ganzzahlk. */
extern unsigned long Buf_size;	/* Buffer für Größenangaben */

extern EfiType Type_char;	/* Zeichenkonstante */
extern EfiType Type_bool;	/* Logische Konstante */
extern EfiType Type_byte;	/* 1 Byte Ganzzahlkonstante */
extern EfiType Type_short;	/* Kurze Ganzzahlkonstante */
extern EfiType Type_int;		/* Normale Ganzzahlkonstante */
extern EfiType Type_long;	/* Lange Ganzzahlkonstante */
extern EfiType Type_uint;	/* Vorzeichenfreie Ganzzahlkonstante */
extern EfiType Type_size;	/* Größenangaben */
extern EfiType Type_Date;	/* Datumsindex */
extern EfiType Type_Time;	/* Zeitindex */

#define	Val_bool(x)	((int *) (x))[0]
#define	Val_char(x)	((unsigned char *) (x))[0]
#define	Val_byte(x)	((char *) (x))[0]
#define	Val_short(x)	((short *) (x))[0]
#define	Val_int(x)	((int *) (x))[0]
#define	Val_long(x)	((long *) (x))[0]
#define	Val_uint(x)	((unsigned *) (x))[0]
#define	Val_size(x)	((size_t *) (x))[0]
#define	Val_Date(x)	((int *) (x))[0]

#define	bool2Obj(x)	NewObj(&Type_bool, (Buf_int = (x), &Buf_int))
#define	byte2Obj(x)	NewObj(&Type_byte, (Buf_byte = (x), &Buf_byte))
#define	char2Obj(x)	NewObj(&Type_char, (Buf_char = (x), &Buf_char))
#define	short2Obj(x)	NewObj(&Type_short, (Buf_short = (x), &Buf_short))
#define	int2Obj(x)	NewObj(&Type_int, (Buf_int = (x), &Buf_int))
#define	long2Obj(x)	NewObj(&Type_long, (Buf_long = (x), &Buf_long))
#define	uint2Obj(x)	NewObj(&Type_uint, (Buf_uint = (x), &Buf_uint))
#define	size2Obj(x)	NewObj(&Type_size, (Buf_size = (x), &Buf_size))

#define	Obj2bool(x)	(Obj2Data((x), &Type_bool, &Buf_int), Buf_int)
#define	Obj2char(x)	(Obj2Data((x), &Type_char, &Buf_char), Buf_char)
#define	Obj2byte(x)	(Obj2Data((x), &Type_byte, &Buf_byte), Buf_byte)
#define	Obj2short(x)	(Obj2Data((x), &Type_short, &Buf_short), Buf_short)
#define	Obj2int(x)	(Obj2Data((x), &Type_int, &Buf_int), Buf_int)
#define	Obj2long(x)	(Obj2Data((x), &Type_long, &Buf_long), Buf_long)
#define	Obj2uint(x)	(Obj2Data((x), &Type_uint, &Buf_uint), Buf_uint)
#define	Obj2size(x)	(Obj2Data((x), &Type_size, &Buf_size), Buf_size)


/*	Gleitkommaobjekte
*/

extern float Buf_float;		/* Buffer für kurze Gleitkommakonstante */
extern double Buf_double;	/* Buffer für Gleitkommakonstante */

extern EfiType Type_float;	/* Kurze Gleitkommakonstante */
extern EfiType Type_double;	/* Gleitkommakonstante */

#define	float2Obj(x)	NewObj(&Type_float, (Buf_float = (x), &Buf_float))
#define	double2Obj(x)	NewObj(&Type_double, (Buf_double = (x), &Buf_double))

#define	Val_float(x)	((float *) (x))[0]
#define	Val_double(x)	((double *) (x))[0]

#define	Obj2float(x)	(Obj2Data((x), &Type_float, &Buf_float), Buf_float)
#define	Obj2double(x)	(Obj2Data((x), &Type_double, &Buf_double), Buf_double)


/*	Pointerobjekte
*/

EfiObj *NewPtrObj (EfiType *type, const void *data);
void *Obj2Ptr (EfiObj *obj, EfiType *type);

extern EfiType Type_ptr;		/* Pointerobjekt */
extern EfiType Type_ref;		/* Referenzobjekt */
extern EfiType Type_enum;	/* Aufzählungsobjekt */
extern EfiType Type_str;		/* Stringkonstante */
extern EfiType Type_obj;		/* Objektpointer */
extern EfiType Type_expr;	/* Objektpointer mit Ausdruck */
extern EfiType Type_type;	/* Datentype */
extern EfiType Type_lval;	/* Lval-Datentype */
extern EfiType Type_info;	/* Informationsknoten */
extern EfiType Type_io;		/* IO - Struktur */
extern EfiType Type_vtab;	/* Variablentabelle */
extern EfiType Type_undef;	/* Nicht definierter Name */
extern EfiType Type_name;	/* Name */

#define	ptr2Obj(x)	NewPtrObj(&Type_ptr, x)
#define	str2Obj(x)	NewPtrObj(&Type_str, x)
#define	obj2Obj(x)	NewPtrObj(&Type_obj, x)
#define	expr2Obj(x)	NewPtrObj(&Type_expr, x)
#define	type2Obj(x)	NewPtrObj(&Type_type, x)
#define	io2Obj(x)	NewPtrObj(&Type_io, x)

#define	Val_ptr(x)	((void **) (x))[0]
#define	Val_str(x)	((char **) (x))[0]
#define	Val_io(x)	((IO **) (x))[0]
#define	Val_obj(x)	((EfiObj **) (x))[0]
#define	Val_xobj(x)	((EfiObj **) (x))[0]
#define	Val_type(x)	((EfiType **) (x))[0]
#define	Val_func(x)	((EfiFunc **) (x))[0]
#define	Val_vfunc(x)	((EfiVirFunc **) (x))[0]
#define	Val_vtab(x)	((EfiVarTab **) (x))[0]

#define	Obj2ptr(x)	Obj2Ptr((x), &Type_ptr)
#define	Obj2str(x)	((char *) Obj2Ptr((x), &Type_str))
#define	Obj2obj(x)	((EfiObj *) Obj2Ptr((x), &Type_obj))


/*	Struktur für Member/Scope - Namensdefinition
*/

typedef struct {
	char *name;
	EfiObj *obj;
} EfiName;

extern EfiName Buf_name;		/* Zwischenbuffer */
extern EfiType Type_mname;	/* Member-Name */
extern EfiType Type_sname;	/* Scope-Name */


/*	Vektoren
*/

EfiObj *Vector (EfiVec *vec, size_t idx);

extern EfiType Type_vec;		/* Vektorstruktur */
extern EfiVec Buf_vec;		/* Buffer für Vektorstruktur */


EfiObj *StructMember (const EfiVar *st, const EfiObj *obj);
EfiObj *ResourceObj (EfiType *type, const char *name);

EfiVar *NewVar (EfiType *type, const char *name, size_t dim);
EfiVar *Obj2Var (const char *name, EfiObj *obj);
void DelVar (EfiVar *var);


/*	Variablenstack
*/

typedef struct EfiVarStackStruct EfiVarStack;

struct EfiVarStackStruct {
	EfiVarStack *next;
	EfiVarTab *tab;
	EfiObj *obj;
};

extern EfiVarStack *VarStack;
extern EfiVarStack *ContextStack;

EfiVarStack *SaveVarStack (void);
void RestoreVarStack (EfiVarStack *stack);

void PushVarTab (EfiVarTab *tab, EfiObj *obj);
void PopVarTab (void);
void PushContext (EfiVarTab *tab, EfiObj *obj);
void PopContext (void);

void AddVarDef (EfiVarTab *tab, EfiVarDef *def, size_t dim);
void AddVar (EfiVarTab *tab, EfiVar *def, size_t dim);
EfiObj *GetVar (EfiVarTab *tab, const char *name, const EfiObj *obj);
void SetVar (EfiVarTab *tab, const char *name, const char *value);

EfiObj *GetMember (EfiObj *obj, const char *name);
EfiObj *GetScope (EfiObj *obj, const char *name);


/*	Objektlisten
*/

typedef struct EfiObjListStruct {
	struct EfiObjListStruct *next;
	EfiObj *obj;
} EfiObjList;

extern EfiType Type_list;

#define	Obj_list(x)	NewPtrObj(&Type_list, x)

#define	Val_list(x)	((EfiObjList **) (x))[0]

EfiObjList *NewObjList (EfiObj *obj);
void DelObjList (EfiObjList *list);
EfiObj *ReduceObjList (EfiObjList **ptr);
void ExpandObjList (EfiObjList **list, EfiObj *obj);
EfiObjList *RefObjList (const EfiObjList *list);
EfiObjList *EvalObjList (const EfiObjList *list);
EfiObjList *MakeObjList (int n, ...);
EfiObjList *Expand_vec (EfiType *type, const void *data, size_t dim);
void Assign_vec (EfiType *type, void *data, size_t dim, const EfiObjList *list);
int ObjListLen (const EfiObjList *list);

typedef struct {
	EfiType *type;	/* Variablentype */
	EfiName name;	/* Namensdefinition */
	int cnst : 1;	/* Konstante */
	EfiObjList *idx; /* Indexliste */
	EfiObj *defval;	/* Initialisierungswert */
} EfiVarDecl;

EfiObjList *VarDefList (IO *io, int delim);
EfiObjList *EnumKeyList (EfiType *type);


typedef struct {
	EfiVarTab *tab;
	EfiObjList *list;
} EfiBlock;

extern EfiType Type_block;	/* Befehlsblock */
extern EfiType Type_vdef;	/* Variablendefinition */


#define	Obj_noop()		Obj_call(NULL, NULL, NULL)

EfiType *VecType(EfiType *type, EfiObjList *idx);
EfiType *NewVecType(EfiType *type, int dim);


/*	Ausdrücke und Terme
*/

typedef struct {
	EfiObj *(*eval) (void *par, const EfiObjList *list);	/* Auswertungsfunktion */
	const void *par;	/* Auswertungsparameter */
	EfiObjList *list;	/* Argumentliste */
} EfiExpr;


extern EfiType Type_explist;
extern EfiType Type_call;
extern EfiType Type_func;
extern EfiType Type_vfunc;
extern EfiType Type_ofunc;

EfiObj *Obj_call (EfiObj *(*func) (void *par, const EfiObjList *list), void *par, EfiObjList *l);

EfiObj *Expr_func (void *par, const EfiObjList *list);
EfiObj *Expr_virfunc (void *par, const EfiObjList *list);
EfiObj *Expr_void (void *par, const EfiObjList *list);
EfiObj *Expr_for (void *par, const EfiObjList *list);
EfiObj *Expr_forin (void *par, const EfiObjList *list);

EfiObj *CondTerm (EfiObj *test, EfiObj *ifpart, EfiObj *elsepart);
EfiObj *CommaTerm (EfiObj *left, EfiObj *right);
EfiObj *UnaryTerm (const char *a, const char *ext, EfiObj *obj);
EfiObj *BinaryTerm (const char *a, EfiObj *left, EfiObj *right);

void RangeFunc (EfiFunc *func, void *rval, void **arg);


/*	Objektkonvertierungen
*/

void Ptr2bool (EfiFunc *func, void *rval, void **arg);
void Vec2List (EfiFunc *func, void *rval, void **arg);
void Struct2List (EfiFunc *func, void *rval, void **arg);
void List2Struct (EfiFunc *func, void *rval, void **arg);

/*	Operatordefinitionen
*/

void StdOpDef (void);

extern NameKeyTab PrefixTab;
extern NameKeyTab PostfixTab;


/*	Funktionsdefinition
*/

typedef struct {
	EfiType *type;		/* Argumenttype */
	char *name;		/* Argumentname */
	EfiObj *defval;		/* Vorgabewert */
	unsigned lval : 1;	/* L-Wert erforderlich */
	unsigned nokonv : 1;	/* Keine Konvertierung erlaubt */
	unsigned cnst : 1;	/* Konstante */
} EfiFuncArg;

#define	KONV_PROMOTION	0
#define	KONV_STANDARD	1
#define	KONV_RESTRICTED	2

#define	FUNC_PROMOTION	0x1	/* Geförderte Konvertierung */
#define	FUNC_RESTRICTED	0x2	/* Eingeschränkte Konvertierung */
#define	FUNC_LRETVAL	0x4	/* Rückgabe eines L-Wertes */
#define	FUNC_XRETVAL	0x8	/* Rückgabe eines Objektpointers */
#define	FUNC_VIRTUAL	0x10	/* Virtuelle Funktion */
#define	FUNC_BOUND	0x20	/* Typegebundene Funktiomn */
#define	FUNC_VAARG	0x40	/* Variable Argumentzahl */
#define	FUNC_INLINE	0x80	/* Inline Funktion, Lokale Variablen sichtbar */

#define	FUNC_BUFSIZE	sizeof(double)

struct EfiFuncStruct {
	REFVAR;	
	char *name;		/* Funktionsname */
	EfiVarTab *scope;	/* Funktionsumgebung */
	EfiType *type;		/* Rückgabetype */
	EfiFuncArg *arg;	/* Argumentliste */
	void (*eval) (EfiFunc *func, void *rval, void **arg);
	unsigned dim : 24;	/* Zahl der Argumente */
	unsigned lretval : 1;	/* Flag für Rückgabe von L-Wert */
	unsigned bound : 1;	/* Flag für Objektbindung */
	unsigned vaarg : 1;	/* Flag für variable Argumentzahl */
	unsigned virfunc : 1;	/* Virtuelle Funktion */
	unsigned weight : 4;	/* Konvertierungsgewicht */
	void *par;		/* Funktionsparameter */
	void (*clean) (void *ptr);		/* Aufräumfunktion */
};

size_t GetFuncArg (IO *io, EfiFuncArg **arg);
void DelFuncArg (EfiFuncArg *arg, size_t dim);
EfiFuncArg *VaFuncArg (int narg, va_list list);
EfiFuncArg *FuncArg (int narg, ...);

int ParseFuncArg(IO *io, EfiFuncArg *ptr);
int IsFunc (void *data);

#define	FARG_END	1	/* Ende der Argumentliste */
#define	FARG_ERROR	2	/* Fehlerhaftes Argument */
#define	FARG_STDARG	3	/* Normales Argument */
#define	FARG_ELLIPSE	4	/* Ellipse */

EfiFunc *MakePrototype (IO *io, EfiType *type, EfiName *name, unsigned flags);
EfiFunc *Prototype (const char *fmt, EfiType *type,
	EfiName *name, unsigned flags);

void SetFunc (unsigned flags, EfiType *type, const char *fmt,
	void (*eval) (EfiFunc *func, void *rval, void **arg));

ArgList *arg_list (const char *base, EfiObjList *list);
ArgList *arg_func (EfiFunc *func, int n, void **arg);

int ListFunc (IO *io, EfiFunc *func);
int FuncComp (const void *a, const void *b);
void FuncDebug (EfiFunc *func, const char *pfx);
extern int FuncDebugLock;


EfiFunc *GetFunc (EfiType *type, EfiVirFunc *tab,
	int narg, ...);
EfiFunc *VaGetFunc (EfiType *type, EfiVirFunc *tab,
	int narg, va_list list);
EfiFunc *XGetFunc (EfiType *type, EfiVirFunc *tab,
	EfiFuncArg *arg, size_t narg);

EfiFunc *NewFunc (void);
void DelFunc (EfiFunc *func);
void AddFunc (EfiFunc *func);

typedef struct {
	EfiObj *obj;	/* Objekt */
	void *func;	/* Funktionspointer */
} EfiObjFunc;


/*	Funktionstabelle
*/

typedef struct {
	unsigned flags;		/* Funktionsflags */
	EfiType *type;		/* Datentype */
	char *prot;		/* Prototype */
	void (*eval) (EfiFunc *func, void *rval, void **arg);	/* Auswertungsfunktion */
} EfiFuncDef;

void AddFuncDef (EfiFuncDef *def, size_t dim);


/*	Funktionen auswerten
*/

void Func_inline (EfiFunc *func, void *rval, void **arg);
void Func_func (EfiFunc *func, void *rval, void **arg);

EfiObj *EvalFunc (EfiFunc *func, const EfiObjList *list);
EfiObj *EvalVirFunc (EfiVirFunc *func, const EfiObjList *list);

EfiObj *MakeRetVal (EfiFunc *func, EfiObj *firstarg, void **arg);
EfiObj *ConstRetVal (EfiFunc *func, void **arg);

void CallFunc (EfiType *type, void *ptr, EfiFunc *func, ...);
void CallVoidFunc (EfiFunc *func, ...);
EfiObj *CallFuncObj (EfiFunc *func, ...);

void *GetTypeFunc (const EfiType *type, const char *name);
void *GetStdFunc (EfiVarTab *tab, const char *name);

#define	GetGlobalFunc(name)	GetStdFunc(GlobalVar, name)


/*	Befehlsinterpreter
*/

void SetupInt (void);
void SetupFloat (void);
void SetupMath (void);

void StdKonvDef (void);
void StdFuncDef (void);
void StdPrintDef (void);
void StdIoDef (void);
void SetupStd (void);
void SetupSearchTab (void);

void CmdEval (IO *in, IO *out);
int iocpy_eval (IO *in, IO *out, int key, const char *arg, unsigned flags);


/*	Parse - Funktionen
*/

char *Parse_name (IO *io);
EfiName *Parse_sname (IO *io, EfiName *buf);
EfiType *Parse_type (IO *io, EfiType *type);
EfiObj *Parse_obj (IO *io, int flags);
EfiObj *Parse_term (IO *io, int prior);
EfiObj *Parse_index (IO *io);
EfiObj *strterm (const char *str);
EfiObjList *Parse_list (IO *io, int endchar);
EfiObjList *Parse_clist (IO *io, int endchar);
EfiObjList *Parse_idx(IO *io);

EfiObj *Parse_op (IO *io, int prior, EfiObj *left);
EfiObj *Parse_cmd (IO *io);

#define	VDEF_REPEAT	0x1	/* Fortgesetzte Definition */
#define	VDEF_LVAL	0x2	/* LVal Flag */
#define	VDEF_CONST	0x4	/* Konstante */

EfiObj *Parse_vdef (IO *io, EfiType *var, int flag);
EfiObj *Parse_func (IO *io, EfiType *type, EfiName *name, int flags);
int Parse_fmt (IO *io, const char *fmt, ...);

/*	Blockstrukturen
*/

EfiObj *Parse_cmd(IO *io);
EfiObj *Parse_block(IO *io, int endchar);

EfiObj *EvalExpression (const EfiObj *obj);

#endif	/* EFEU/object.h */

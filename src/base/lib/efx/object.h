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

#ifndef	EFEU_OBJECT_H
#define	EFEU_OBJECT_H	1

#include <EFEU/efmain.h>
#include <EFEU/efio.h>
#include <EFEU/vecbuf.h>
#include <EFEU/data.h>

/*	Typedefinitionen für Objekte
*/

typedef struct Obj_s Obj_t;
typedef struct Type_s Type_t;
typedef struct Func_s Func_t;
typedef struct Lval_s Lval_t;


/*	Vektoren
*/

typedef struct {
	Type_t *type;	/* Variablentype */
	void *data;	/* Datenpointer */
	size_t dim;	/* Vektordimension */
} Vec_t;


/*	Variablen
*/

typedef struct Var_s Var_t;
typedef Obj_t *(*GetMember_t) (const Var_t *member, const Obj_t *obj);

struct Var_s {
	char *name;		/* Variablenname */
	Type_t *type;		/* Variablentype */
	void *data;		/* Datenpointer */
	size_t dim;		/* Variablendimension */
	size_t offset;		/* Datenoffset */
	GetMember_t member;	/* Abfragefunktion */
	void *par;		/* Strukturparameter */
	Var_t *next;		/* Nächster Eintrag */
	char *desc;		/* Beschreibungstext */
	clean_t clean;		/* Aufräumfunktion */
};

Obj_t *Var2Obj (Var_t *var, const Obj_t *obj);


/*	Variablendefinition
*/

typedef struct {
	char *name;		/* Variablenname */
	Type_t *type;		/* Variablentype */
	void *data;		/* Datenpointer */
	char *desc;		/* Beschreibungstext */
} VarDef_t;

/*	Mitgliederdefinitionen
*/

typedef struct {
	char *name;		/* Variablenname */
	Type_t *type;		/* Variablentype */
	GetMember_t member;	/* Abfragefunktion */
	void *par;		/* Strukturparameter */
	char *desc;		/* Beschreibungstext */
} MemberDef_t;

/*	Variablentabellen
*/

typedef struct {
	REFVAR;
	char *name;
	xtab_t tab;
} VarTab_t;


extern reftype_t VarTabRefType;

extern VarTab_t *GlobalVar;
extern VarTab_t *LocalVar;
extern VarTab_t *ContextVar;
extern Obj_t *LocalObj;
extern Obj_t *ContextObj;

VarTab_t *VarTab (const char *name, size_t dim);
int ShowVarTab (io_t *io, const char *pfx, VarTab_t *vtab);

#define	RefVarTab(tab)	rd_refer(tab)
#define	DelVarTab(tab)	rd_deref(tab)

/*	Virtuelle Funktionen
*/

typedef struct {
	REFVAR;
	vecbuf_t tab;
} VirFunc_t;

extern reftype_t VirFuncRefType;

VirFunc_t *VirFunc(Func_t *func);


/*	Objekttype
*/

typedef Obj_t *(*Eval_t) (const Type_t *type, const void *src);
typedef void (*Clean_t) (const Type_t *type, void *tg);
typedef void (*Copy_t) (const Type_t *type, void *tg, const void *src);
typedef size_t (*IOData_t) (const Type_t *t, iofunc_t fdnc, void *ptr,
	void *data, size_t dim);


struct Type_s {
	char *name;	/* Typename */
	size_t size;	/* Datenlänge */
	size_t recl;	/* Satzlänge */
	IOData_t iodata;	/* Ein/Ausgabefunktion */
	Type_t *base;	/* Basisobjekt (Vektor, Vererbung) */
	Eval_t eval;	/* Auswertungsfunktion */
	Clean_t clean;	/* Löschfunktion */
	Copy_t copy;	/* Kopierfunktion */
	size_t dim;	/* Vektordimension */
	Var_t *list;	/* Strukturelemente */
	VarTab_t *vtab;	/* Variablentabelle */
	Func_t *fclean; /* Destruktor, nicht überladbar */
	Func_t *fcopy;	/* Kopierfunktion, nicht überladbar */
	vecbuf_t konv;	/* Konvertierungen */
	VirFunc_t *create;	/* Konstruktor, überladbar */
	void *defval;	/* Vorgabewert */
};


/*	Zuweisungsobjekte
*/

struct Lval_s {
	Obj_t *(*alloc) (Type_t *type, va_list list);
	void (*free) (Obj_t *obj);
	void (*update) (Obj_t *obj);
	void (*sync) (Obj_t *obj);
	char *(*ident) (Obj_t *obj);
};

extern Lval_t Lval_ptr;
extern Lval_t Lval_ref;

struct Obj_s {
	REFVAR;		/* Referenzvariablen */
	Type_t *type;	/* Datentype */
	Lval_t *lval;	/* Lval-Type */
	void *data;	/* Datenpointer */
};

extern Obj_t *Obj_alloc (size_t size);
extern void Obj_free (Obj_t *obj, size_t size);
extern void Obj_stat (const char *prompt);
extern void Obj_check (const Obj_t *obj);

Type_t *NewType (char *name);


/*	Makro zur Typedefinition
*/

void AddType (Type_t *type);
void AliasType (char *name, Type_t *type);
Type_t *GetType (const char *name);


/*	Initialisierungsfunktionen
*/

Var_t *GetStruct (io_t *io, int delim);
Type_t *FindStruct (Var_t *list, size_t size);
Type_t *MakeStruct (char *name, Var_t *base, Var_t *list);

Obj_t *LvalObj (Lval_t *lval, Type_t *type, ...);
Obj_t *NewObj (Type_t *type, void *data);
Obj_t *ConstObj (Type_t *type, const void *data);
Obj_t *RefObj (const Obj_t *obj);
void DeleteObj (Obj_t *obj);
void UnrefObj (Obj_t *obj);
void SyncLval (Obj_t *obj);
Obj_t *AssignTerm (const char *name, Obj_t *left, Obj_t *right);
Obj_t *AssignObj (Obj_t *left, Obj_t *right);

Obj_t *EvalObj (Obj_t *obj, Type_t *type);
void UnrefEval (Obj_t *obj);

int TypeComp (const Type_t *def1, const Type_t *def2);

size_t IOData (const Type_t *type, iofunc_t func, void *ptr, void *dat);
void CleanData (const Type_t *type, void *tg);
void CopyData (const Type_t *type, void *tg, const void *src);
void AssignData (const Type_t *type, void *tg, const void *src);

size_t IOVecData (const Type_t *type, iofunc_t f, void *p, size_t n, void *dat);
void CleanVecData (const Type_t *type, size_t dim, void *tg);
void CopyVecData (const Type_t *type, size_t dim, void *tg, const void *src);
void AssignVecData (const Type_t *type, size_t dim, void *tg, const void *src);

void Obj2Data (Obj_t *obj, Type_t *type, void *ptr);

int ShowType (io_t *io, const Type_t *type);
void ListType (io_t *io, const Type_t *type);

extern Type_t Type_void;	/* Leerer Type */


/*	Ganzzahlobjekte
*/

extern uchar_t Buf_char;	/* Buffer für Zeichenkonstante */
extern char Buf_byte;		/* Buffer für Byteswert */
extern short Buf_short;		/* Buffer für kurze Ganzzahlkonstante */
extern int Buf_int;		/* Buffer für normale Ganzzahlkonstante */
extern long Buf_long;		/* Buffer für lange Ganzzahlkonstante */
extern unsigned Buf_uint;	/* Buffer für Vorzeichenfreie Ganzzahlk. */
extern unsigned long Buf_size;	/* Buffer für Größenangaben */

extern Type_t Type_char;	/* Zeichenkonstante */
extern Type_t Type_bool;	/* Logische Konstante */
extern Type_t Type_byte;	/* 1 Byte Ganzzahlkonstante */
extern Type_t Type_short;	/* Kurze Ganzzahlkonstante */
extern Type_t Type_int;		/* Normale Ganzzahlkonstante */
extern Type_t Type_long;	/* Lange Ganzzahlkonstante */
extern Type_t Type_uint;	/* Vorzeichenfreie Ganzzahlkonstante */
extern Type_t Type_size;	/* Größenangaben */
extern Type_t Type_Date;	/* Datumsindex */
extern Type_t Type_Time;	/* Zeitindex */

#define	Val_bool(x)	((int *) (x))[0]
#define	Val_char(x)	((uchar_t *) (x))[0]
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

extern Type_t Type_float;	/* Kurze Gleitkommakonstante */
extern Type_t Type_double;	/* Gleitkommakonstante */

#define	float2Obj(x)	NewObj(&Type_float, (Buf_float = (x), &Buf_float))
#define	double2Obj(x)	NewObj(&Type_double, (Buf_double = (x), &Buf_double))

#define	Val_float(x)	((float *) (x))[0]
#define	Val_double(x)	((double *) (x))[0]

#define	Obj2float(x)	(Obj2Data((x), &Type_float, &Buf_float), Buf_float)
#define	Obj2double(x)	(Obj2Data((x), &Type_double, &Buf_double), Buf_double)


/*	Pointerobjekte
*/

Obj_t *NewPtrObj (Type_t *type, const void *data);
void *Obj2Ptr (Obj_t *obj, Type_t *type);

extern Type_t Type_ptr;		/* Pointerobjekt */
extern Type_t Type_ref;		/* Referenzobjekt */
extern Type_t Type_str;		/* Stringkonstante */
extern Type_t Type_obj;		/* Objektpointer */
extern Type_t Type_expr;	/* Objektpointer mit Ausdruck */
extern Type_t Type_type;	/* Datentype */
extern Type_t Type_info;	/* Informationsknoten */
extern Type_t Type_io;		/* IO - Struktur */
extern Type_t Type_vtab;	/* Variablentabelle */
extern Type_t Type_undef;	/* Nicht definierter Name */
extern Type_t Type_name;	/* Name */

#define	ptr2Obj(x)	NewPtrObj(&Type_ptr, x)
#define	str2Obj(x)	NewPtrObj(&Type_str, x)
#define	obj2Obj(x)	NewPtrObj(&Type_obj, x)
#define	expr2Obj(x)	NewPtrObj(&Type_expr, x)
#define	type2Obj(x)	NewPtrObj(&Type_type, x)
#define	io2Obj(x)	NewPtrObj(&Type_io, x)

#define	Val_ptr(x)	((void **) (x))[0]
#define	Val_str(x)	((char **) (x))[0]
#define	Val_io(x)	((io_t **) (x))[0]
#define	Val_obj(x)	((Obj_t **) (x))[0]
#define	Val_xobj(x)	((Obj_t **) (x))[0]
#define	Val_type(x)	((Type_t **) (x))[0]
#define	Val_func(x)	((Func_t **) (x))[0]
#define	Val_vfunc(x)	((VirFunc_t **) (x))[0]
#define	Val_vtab(x)	((VarTab_t **) (x))[0]

#define	Obj2ptr(x)	Obj2Ptr((x), &Type_ptr)
#define	Obj2str(x)	((char *) Obj2Ptr((x), &Type_str))
#define	Obj2obj(x)	((Obj_t *) Obj2Ptr((x), &Type_obj))


/*	Struktur für Member/Scope - Namensdefinition
*/

typedef struct {
	char *name;
	Obj_t *obj;
} Name_t;

extern Name_t Buf_name;		/* Zwischenbuffer */
extern Type_t Type_mname;	/* Member-Name */
extern Type_t Type_sname;	/* Scope-Name */


/*	Vektoren
*/

Obj_t *Vector (Vec_t *vec, size_t idx);

extern Type_t Type_vec;		/* Vektorstruktur */
extern Vec_t Buf_vec;		/* Buffer für Vektorstruktur */


Obj_t *StructMember (const Var_t *st, const Obj_t *obj);
Obj_t *LvalMember (const Var_t *st, const Obj_t *obj);
Obj_t *ConstMember (const Var_t *st, const Obj_t *obj);
Obj_t *TransMember (const Var_t *st, const Obj_t *obj);

typedef struct {
	void (*get) (void *data, void *buf);
	void (*set) (void *data, void *buf);
} TransPar_t;

Obj_t *ResourceVar (const Var_t *st, const Obj_t *obj);
Obj_t *ResourceObj (Type_t *type, const char *name);


typedef void *(*EvalMember_t) (const void *data);

Var_t *NewVar (Type_t *type, const char *name, size_t dim);
void DelVar (Var_t *var);


/*	Variablenstack
*/

typedef struct VarStack_s VarStack_t;

struct VarStack_s {
	VarStack_t *next;
	VarTab_t *tab;
	Obj_t *obj;
};

extern VarStack_t *VarStack;
extern VarStack_t *ContextStack;

VarStack_t *SaveVarStack (void);
void RestoreVarStack (VarStack_t *stack);

void PushVarTab (VarTab_t *tab, Obj_t *obj);
void PopVarTab (void);
void PushContext (VarTab_t *tab, Obj_t *obj);
void PopContext (void);

void AddVarDef (VarTab_t *tab, VarDef_t *def, size_t dim);
void AddMember (VarTab_t *tab, MemberDef_t *def, size_t dim);
void AddVar (VarTab_t *tab, Var_t *def, size_t dim);
Obj_t *GetVar (VarTab_t *tab, const char *name, const Obj_t *obj);
void SetVar (VarTab_t *tab, const char *name, const char *value);

Obj_t *GetMember (Obj_t *obj, const char *name);
Obj_t *GetScope (Obj_t *obj, const char *name);


/*	Objektlisten
*/

typedef struct ObjList_s {
	struct ObjList_s *next;
	Obj_t *obj;
} ObjList_t;

extern Type_t Type_list;

#define	Obj_list(x)	NewPtrObj(&Type_list, x)

#define	Val_list(x)	((ObjList_t **) (x))[0]

ObjList_t *NewObjList (Obj_t *obj);
void DelObjList (ObjList_t *list);
Obj_t *ReduceObjList (ObjList_t **ptr);
void ExpandObjList (ObjList_t **list, Obj_t *obj);
ObjList_t *RefObjList (const ObjList_t *list);
ObjList_t *EvalObjList (const ObjList_t *list);
ObjList_t *MakeObjList (int n, ...);
ObjList_t *Expand_vec (Type_t *type, const void *data, size_t dim);
void Assign_vec (Type_t *type, void *data, size_t dim, const ObjList_t *list);
int ObjListLen (const ObjList_t *list);

typedef struct {
	Type_t *type;	/* Variablentype */
	Name_t name;	/* Namensdefinition */
	ObjList_t *idx;	/* Indexliste */
	Obj_t *defval;	/* Initialisierungswert */
} VarDecl_t;

ObjList_t *VarDefList (io_t *io, int delim);
void DelType (Type_t *type);


typedef struct {
	VarTab_t *tab;
	ObjList_t *list;
} Block_t;

extern Type_t Type_block;	/* Befehlsblock */
extern Type_t Type_vdef;	/* Variablendefinition */


#define	Obj_noop()		Obj_call(NULL, NULL, NULL)

Type_t *VecType(Type_t *type, ObjList_t *idx);
Type_t *NewVecType(Type_t *type, int dim);


/*	Ausdrücke und Terme
*/

typedef Obj_t *(*EvalExpr_t) (void *par, const ObjList_t *list);

typedef struct {
	EvalExpr_t eval;	/* Auswertungsfunktion */
	const void *par;	/* Auswertungsparameter */
	ObjList_t *list;	/* Argumentliste */
} Expr_t;


extern Type_t Type_explist;
extern Type_t Type_call;
extern Type_t Type_func;
extern Type_t Type_vfunc;
extern Type_t Type_ofunc;

Obj_t *Obj_call (EvalExpr_t func, void *par, ObjList_t *l);

Obj_t *Expr_func (void *par, const ObjList_t *list);
Obj_t *Expr_virfunc (void *par, const ObjList_t *list);
Obj_t *Expr_void (void *par, const ObjList_t *list);
Obj_t *Expr_for (void *par, const ObjList_t *list);
Obj_t *Expr_forin (void *par, const ObjList_t *list);

Obj_t *CondTerm (Obj_t *test, Obj_t *ifpart, Obj_t *elsepart);
Obj_t *CommaTerm (Obj_t *left, Obj_t *right);
Obj_t *UnaryTerm (const char *a, Obj_t *obj, int flag);
Obj_t *BinaryTerm (const char *a, Obj_t *left, Obj_t *right);


/*	Objektkonvertierungen
*/

void Ptr2bool (Func_t *func, void *rval, void **arg);
void Vec2List (Func_t *func, void *rval, void **arg);
void Struct2List (Func_t *func, void *rval, void **arg);
void List2Struct (Func_t *func, void *rval, void **arg);


/*	Operatordefinitionen
*/

void StdOpDef (void);

extern xtab_t PrefixTab;
extern xtab_t PostfixTab;


/*	Funktionsdefinition
*/

typedef struct {
	Type_t *type;		/* Argumenttype */
	char *name;		/* Argumentname */
	Obj_t *defval;		/* Vorgabewert */
	unsigned lval : 1;	/* L-Wert erforderlich */
	unsigned nokonv : 1;	/* Keine Konvertierung erlaubt */
	unsigned cnst : 1;	/* Konstante */
} FuncArg_t;

typedef void (*FuncEval_t) (Func_t *func, void *rval, void **arg);

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

extern reftype_t FuncRefType;

struct Func_s {
	REFVAR;	
	char *name;		/* Funktionsname */
	VarTab_t *scope;	/* Funktionsumgebung */
	Type_t *type;		/* Rückgabetype */
	FuncArg_t *arg;		/* Argumentliste */
	FuncEval_t eval;	/* Auswertungsfunktion */
	unsigned dim : 24;	/* Zahl der Argumente */
	unsigned lretval : 1;	/* Flag für Rückgabe von L-Wert */
	unsigned bound : 1;	/* Flag für Objektbindung */
	unsigned vaarg : 1;	/* Flag für variable Argumentzahl */
	unsigned virtual : 1;	/* Virtuelle Funktion */
	unsigned weight : 4;	/* Konvertierungsgewicht */
	void *par;		/* Funktionsparameter */
	clean_t clean;		/* Aufräumfunktion */
};

size_t GetFuncArg (io_t *io, FuncArg_t **arg);
void DelFuncArg (FuncArg_t *arg, size_t dim);
FuncArg_t *VaFuncArg (int narg, va_list list);
FuncArg_t *FuncArg (int narg, ...);

int ParseFuncArg(io_t *io, FuncArg_t *ptr);

#define	FARG_END	1	/* Ende der Argumentliste */
#define	FARG_ERROR	2	/* Fehlerhaftes Argument */
#define	FARG_STDARG	3	/* Normales Argument */
#define	FARG_ELLIPSE	4	/* Ellipse */

Func_t *MakePrototype (io_t *io, Type_t *type, Name_t *name, unsigned flags);
Func_t *Prototype (const char *fmt, Type_t *type, Name_t *name, unsigned flags);

void SetFunc (unsigned flags, Type_t *type, const char *fmt, FuncEval_t eval);

int ListFunc (io_t *io, Func_t *func);
int FuncComp (const void *a, const void *b);
void FuncDebug (Func_t *func, const char *pfx);
extern int FuncDebugLock;


Func_t *GetFunc (Type_t *type, VirFunc_t *tab, int narg, ...);
Func_t *VaGetFunc (Type_t *type, VirFunc_t *tab, int narg, va_list list);
Func_t *XGetFunc (Type_t *type, VirFunc_t *tab, FuncArg_t *arg, size_t narg);

Func_t *NewFunc (void);
void DelFunc (Func_t *func);
void AddFunc (Func_t *func);

typedef struct {
	Obj_t *obj;	/* Objekt */
	void *func;	/* Funktionspointer */
} ObjFunc_t;


/*	Funktionstabelle
*/

typedef struct {
	unsigned flags;		/* Funktionsflags */
	Type_t *type;		/* Datentype */
	char *prot;		/* Prototype */
	FuncEval_t eval;	/* Auswertungsfunktion */
} FuncDef_t;

void AddFuncDef (FuncDef_t *def, size_t dim);


/*	Funktionen auswerten
*/

void Func_inline (Func_t *func, void *rval, void **arg);
void Func_func (Func_t *func, void *rval, void **arg);

Obj_t *EvalFunc (Func_t *func, const ObjList_t *list);
Obj_t *EvalVirFunc (VirFunc_t *func, const ObjList_t *list);

Obj_t *MakeRetVal (Func_t *func, Obj_t *firstarg, void **arg);
Obj_t *ConstRetVal (Func_t *func, void **arg);

void CallFunc (Type_t *type, void *ptr, Func_t *func, ...);
void CallVoidFunc (Func_t *func, ...);
Obj_t *CallFuncObj (Func_t *func, ...);

void *GetTypeFunc (const Type_t *type, const char *name);
void *GetStdFunc (VarTab_t *tab, const char *name);

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
void SetupSC (void);
void SetupSearchTab (void);

void CmdEval (io_t *in, io_t *out);


/*	Parse - Funktionen
*/

char *Parse_name (io_t *io);
Name_t *Parse_sname (io_t *io, Name_t *buf);
Type_t *Parse_type (io_t *io, Type_t *type);
Obj_t *Parse_obj (io_t *io, int flags);
Obj_t *Parse_term (io_t *io, int prior);
Obj_t *Parse_index (io_t *io);
Obj_t *strterm (const char *str);
ObjList_t *Parse_list (io_t *io, int endchar);
ObjList_t *Parse_clist (io_t *io, int endchar);
ObjList_t *Parse_idx(io_t *io);

Obj_t *Parse_op (io_t *io, int prior, Obj_t *left);
Obj_t *Parse_cmd (io_t *io);

Obj_t *Parse_vdef (io_t *io, Type_t *var, int flag);
Obj_t *Parse_func (io_t *io, Type_t *type, Name_t *name, int flags);
int Parse_fmt (io_t *io, const char *fmt, ...);

/*	Blockstrukturen
*/

Obj_t *Parse_cmd(io_t *io);
Obj_t *Parse_block(io_t *io, int endchar);

Obj_t *EvalExpression (const Obj_t *obj);

#endif	/* EFEU_OBJECT_H */

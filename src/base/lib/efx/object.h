/*
Objektdefinitionen

$Header <EFEU/$1>

$Copyright (C) 1994 Erich Fr�hst�ck
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
#include <EFEU/stdint.h>

/*	Typedefinitionen f�r Objekte
*/

typedef struct EfiObjStruct EfiObj;
typedef struct EfiTypeStruct EfiType;
typedef struct EfiFuncStruct EfiFunc;
typedef struct EfiLvalStruct EfiLval;

/*	interpreter status (opaque type)
*/

typedef struct EfiStat Efi;
extern Efi *Efi_ptr (Efi *ptr);

/*	code source
*/

typedef struct {
	REFVAR;		/* reference variables */
	char *cmd;	/* load command */
} EfiSrc;

void EfiSrc_lib (Efi *efi, const char *name, const char *init);
void EfiSrc_hdr (Efi *efi, const char *name);
void EfiSrc_pop (Efi *efi);

/*	Vektoren
*/

typedef struct {
	REFVAR;		/* Referenzvariablen */
	EfiType *type;	/* Variablentype */
	VecBuf buf;	/* Datenbuffer */
} EfiVec;

EfiVec *NewEfiVec (EfiType *type, void *data, size_t dim);
EfiObj *EfiVecObj (EfiType *type, void *data, size_t dim);
EfiObj *Vector (EfiVec *vec, size_t idx);
void EfiVec_resize (EfiVec *vec, size_t dim);
void EfiVec_append (EfiVec *vec, EfiObj *obj);
void EfiVec_delete (EfiVec *vec, size_t pos, size_t dim);
EfiVec *EfiVec_copy (EfiVec *base);

extern EfiType Type_vec;


/*	Variablen
*/

typedef struct EfiVarStruct EfiVar;

struct EfiVarStruct {
	REFVAR;
	char *name;		/* Variablenname */
	EfiType *type;		/* Variablentype */
	void *data;		/* Datenpointer */
	size_t dim;		/* Variablendimension */
	size_t offset;		/* Datenoffset */
	EfiObj *(*member) (const EfiVar *member, const EfiObj *obj);
	void *par;		/* Strukturparameter */
	EfiVar *next;		/* N�chster Eintrag */
	char *desc;		/* Beschreibungstext */
	void (*clean) (void *ptr);	/* Aufr�umfunktion */
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
VarTabEntry *VarTab_xget (EfiVarTab *tab, const char *name);
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
void AddTypeMember (EfiType *type, size_t offset, size_t dim,
	const char *vtype, const char *vname, const char *desc);
void AddMemberFunc (EfiType *type);

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
	char *name;		/* Typename */
	char *cname;		/* C-Datentyp */
	char *desc;		/* Beschreibungstext */
	size_t size;		/* Datenl�nge */
	size_t recl;		/* Satzl�nge */
	size_t (*read) (const EfiType *type, void *data, IO *io);
	size_t (*write) (const EfiType *type, const void *data, IO *io);
	int (*print) (const EfiType *type, const void *data, IO *io);
	EfiType *base;		/* Basisobjekt (Vektor, Vererbung) */
	EfiObj *(*eval) (const EfiType *type, const void *src);
	void (*destroy) (const EfiType *type, void *tg);
	void (*clean) (const EfiType *type, void *tg);
	void (*copy) (const EfiType *type, void *tg, const void *src);
	size_t order;		/* Ordnungsindex */
	size_t dim;		/* Vektordimension */
	EfiVar *list;		/* Strukturelemente */
	EfiVarTab *vtab;	/* Variablentabelle */
	EfiFunc *fclean;	/* Destruktor, nicht �berladbar */
	EfiFunc *fcopy;		/* Kopierfunktion, nicht �berladbar */
	VecBuf konv;		/* Konvertierungen */
	VecBuf par;		/* Parametertabelle */
	EfiVirFunc *create;	/* Konstruktor, �berladbar */
	void *defval;		/* Vorgabewert */
	EfiSrc *src;		/* Quelle */
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

extern EfiLval Lval_data;
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
void DelType (EfiType *type);

typedef struct {
	char *name;
	int val;
} EnumTypeDef;

EfiType *NewEnumType (const char *name);
EfiType *AddEnumType(EfiType *type);
EfiType *EnumType (const char *name);
EfiType *FindEnum (EfiType *base);

void AddEnumKey (EfiType *type, char *name, char *desc, int val);
void MakeEnumType (const char *name, EnumTypeDef *def, size_t dim);
char *EnumKeyLabel (const EfiType *type, int val, int flag);
int EnumKeyCode (const EfiType *type, const char *key);
int NextEnumCode (const EfiType *type, int start);


/*	Makro zur Typedefinition
*/

void AddType (EfiType *type);
EfiType *GetType (const char *name);
EfiType *XGetType (const char *name);
int IsTypeClass (const EfiType *type, const EfiType *base);


/*	Initialisierungsfunktionen
*/

EfiVar *GetStruct (IO *io, int delim);
EfiVar *GetStructEntry (IO *io, EfiType *type);
EfiType *FindStruct (EfiVar *list, size_t size);
EfiType *MakeStruct (char *name, EfiVar *base, EfiVar *list);

EfiObj *LvalObj (EfiLval *lval, EfiType *type, ...);
EfiObj *NewObj (EfiType *type, void *data);
EfiObj *ConstObj (EfiType *type, const void *data);
EfiObj *RefObj (const EfiObj *obj);
void DeleteObj (EfiObj *obj);
void UnrefObj (EfiObj *obj);
void SyncLval (EfiObj *obj);
void UpdateLval (EfiObj *obj);
EfiObj *AssignTerm (const char *name, EfiObj *left, EfiObj *right);
EfiObj *AssignObj (EfiObj *left, EfiObj *right);

EfiObj *EvalObj (EfiObj *obj, EfiType *type);
void UnrefEval (EfiObj *obj);

int TypeComp (const EfiType *def1, const EfiType *def2);

void DestroyData (const EfiType *type, void *tg);
void CleanData (const EfiType *type, void *tg);
void CopyData (const EfiType *type, void *tg, const void *src);
size_t ReadData (const EfiType *type, void *data, IO *io);
size_t WriteData (const EfiType *type, const void *data, IO *io);
void AssignData (const EfiType *type, void *tg, const void *src);

void DestroyVecData (const EfiType *type, size_t dim, void *tg);
void CleanVecData (const EfiType *type, size_t dim, void *tg);
void CopyVecData (const EfiType *type, size_t dim, void *tg, const void *src);
size_t ReadVecData (const EfiType *type, size_t dim, void *data, IO *io);
size_t WriteVecData (const EfiType *type, size_t dim, const void *data, IO *io);
void AssignVecData (const EfiType *type, size_t dim, void *tg, const void *src);

void Obj2Data (EfiObj *obj, EfiType *type, void *ptr);
void Obj2VecData (EfiObj *obj, EfiType *type, size_t dim, void *ptr);

int ShowType (IO *io, const EfiType *type);
void TypeTree (IO *io, const EfiType *type);
char *TypeHead (const EfiType *type);

extern EfiType Type_void;	/* Leerer Type */


/*	Ganzzahlobjekte
*/

extern EfiType Type_char;	/* Zeichenkonstante */
extern EfiType Type_bool;	/* Logische Konstante */
extern EfiType Type_int;	/* Normale Ganzzahlkonstante */
extern EfiType Type_uint;	/* Vorzeichenfreie Ganzzahlkonstante */
extern EfiType Type_Date;	/* Datumsindex */
extern EfiType Type_Time;	/* Zeitindex */
extern EfiType Type_strbuf;	/* Stringbuffer */
extern EfiType Type_varint;	/* Variable Ganzzahlkonstante */
extern EfiType Type_varsize;	/* Vorzeichenfreie, variable Ganzzahlk. */

extern EfiType Type_int8;
extern EfiType Type_int16;
extern EfiType Type_int32;
extern EfiType Type_int64;

extern EfiType Type_uint8;
extern EfiType Type_uint16;
extern EfiType Type_uint32;
extern EfiType Type_uint64;

#define	Val_bool(x)	((int *) (x))[0]
#define	Val_char(x)	((unsigned char *) (x))[0]
#define	Val_int(x)	((int *) (x))[0]
#define	Val_uint(x)	((unsigned *) (x))[0]
#define	Val_Date(x)	((int *) (x))[0]

EfiObj *bool2Obj (int val);
EfiObj *char2Obj (int val);
EfiObj *int2Obj (int val);
EfiObj *uint2Obj (unsigned val);
EfiObj *varint2Obj (int64_t val);
EfiObj *varsize2Obj (uint64_t val);

int Obj2bool (EfiObj *obj);
int Obj2char (EfiObj *obj);
int Obj2int (EfiObj *obj);
unsigned Obj2uint (EfiObj *obj);


/*	Gleitkommaobjekte
*/

extern EfiType Type_float;	/* Kurze Gleitkommakonstante */
extern EfiType Type_double;	/* Gleitkommakonstante */

EfiObj *float2Obj (double val);
EfiObj *double2Obj (double val);

float Obj2float (EfiObj *obj);
double Obj2double (EfiObj *obj);

#define	Val_float(x)	((float *) (x))[0]
#define	Val_double(x)	((double *) (x))[0]

/*	Pointerobjekte
*/

EfiObj *NewPtrObj (EfiType *type, const void *data);
void *Obj2Ptr (EfiObj *obj, EfiType *type);

extern EfiType Type_ptr;		/* Pointerobjekt */
extern EfiType Type_ref;		/* Referenzobjekt */
extern EfiType Type_efi;		/* Interpreterobjekt */
extern EfiType Type_enum;	/* Aufz�hlungsobjekt */
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


/*	Struktur f�r Member/Scope - Namensdefinition
*/

typedef struct {
	char *name;
	EfiObj *obj;
} EfiName;

extern EfiName Buf_name;		/* Zwischenbuffer */
extern EfiType Type_mname;	/* Member-Name */
extern EfiType Type_sname;	/* Scope-Name */


/*	Variablen
*/


EfiObj *StructMember (const EfiVar *st, const EfiObj *obj);
EfiObj *ResourceObj (EfiType *type, const char *name);

EfiVar *NewVar (EfiType *type, const char *name, size_t dim);
EfiVar *Obj2Var (const char *name, EfiObj *obj);
void DelVarList (EfiVar *var);
EfiVar *RefVarList (const EfiVar *var);

#define	DelVar(var)	rd_deref(var)

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
void UpdateObjList (EfiObjList *list);
void SyncObjList (EfiObjList *list);
EfiObj *ReduceObjList (EfiObjList **ptr);
void ExpandObjList (EfiObjList **list, EfiObj *obj);
EfiObjList *RefObjList (const EfiObjList *list);
EfiObjList *EvalObjList (const EfiObjList *list);
EfiObjList *MakeObjList (int n, ...);
EfiObjList *Expand_vec (EfiType *type, const void *data, size_t dim);
void Assign_vec (EfiType *type, void *data, size_t dim, const EfiObjList *list);
int ObjListLen (const EfiObjList *list);

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


/*	Ausdr�cke und Terme
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
void CopyDataFunc (EfiFunc *func, void *rval, void **arg);

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
	unsigned cnst : 1;	/* Konstante */
	unsigned promote : 1;	/* Gef�rderte Konvertierung */
	unsigned nokonv : 29;	/* Keine Konvertierung erlaubt */
} EfiFuncArg;

#define	KONV_PROMOTION	0
#define	KONV_STANDARD	1
#define	KONV_RESTRICTED	2

#define	FUNC_PROMOTION	0x1	/* Gef�rderte Konvertierung */
#define	FUNC_RESTRICTED	0x2	/* Eingeschr�nkte Konvertierung */
#define	FUNC_LRETVAL	0x4	/* R�ckgabe eines L-Wertes */
#define	FUNC_XRETVAL	0x8	/* R�ckgabe eines Objektpointers */
#define	FUNC_VIRTUAL	0x10	/* Virtuelle Funktion */
#define	FUNC_BOUND	0x20	/* Typegebundene Funktiomn */
#define	FUNC_VAARG	0x40	/* Variable Argumentzahl */
#define	FUNC_INLINE	0x80	/* Inline Funktion, Lokale Variablen sichtbar */

#define	FUNC_BUFSIZE	sizeof(double)

struct EfiFuncStruct {
	REFVAR;	
	char *name;		/* Funktionsname */
	EfiVarTab *scope;	/* Funktionsumgebung */
	EfiType *type;		/* R�ckgabetype */
	EfiFuncArg *arg;	/* Argumentliste */
	void (*eval) (EfiFunc *func, void *rval, void **arg);
	unsigned dim : 24;	/* Zahl der Argumente */
	unsigned lretval : 1;	/* Flag f�r R�ckgabe von L-Wert */
	unsigned bound : 1;	/* Flag f�r Objektbindung */
	unsigned vaarg : 1;	/* Flag f�r variable Argumentzahl */
	unsigned virfunc : 1;	/* Virtuelle Funktion */
	unsigned weight : 4;	/* Konvertierungsgewicht */
	void *par;		/* Funktionsparameter */
	void (*clean) (void *ptr);		/* Aufr�umfunktion */
};

size_t GetFuncArg (IO *io, EfiFuncArg **arg, int delim);
int PrintFuncArg (IO *io, EfiFuncArg *arg);
void DelFuncArg (EfiFuncArg *arg, size_t dim);
EfiFuncArg *VaFuncArg (int narg, va_list list);
EfiFuncArg *FuncArg (int narg, ...);

int ParseFuncArg(IO *io, EfiFuncArg *ptr, int delim);
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

EfiFunc *ConstructFunc (EfiType *type, const char *name, const char *def);


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
void CallVoidFuncVec (EfiFunc *func, void **args);
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

char *Parse_name (IO *io, int flag);
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

/*	Zugriff auf Bit-Felder
*/

typedef struct {
	const char *name;
	const char *desc;
	void (*update) (unsigned *data, void *base);
	void (*sync) (unsigned *data, void *base);
} EfiBFMember;

void AddEfiBFMember (EfiVarTab *tab, EfiBFMember *def, size_t dim);

/*	Blockstrukturen
*/

EfiObj *Parse_cmd(IO *io);
EfiObj *Parse_block(IO *io, int endchar);

EfiObj *EvalExpression (const EfiObj *obj);

#endif	/* EFEU/object.h */

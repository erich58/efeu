/*	Informationsdatenbank
	(c) 1998 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.6

$Header	<EFEU/$1>
*/

#ifndef	EFEU_INFO_H
#define	EFEU_INFO_H	1

#include <EFEU/vecbuf.h>
#include <EFEU/io.h>

/*	Info - Datenstruktur: 1. Element ist Ausgabefunktion
*/

typedef struct Info_s Info_t;

typedef void (*InfoPrint_t) (io_t *io, const Info_t *data);

typedef struct {
	admin_t admin;		/* Speicheradministration */
	InfoPrint_t pfunc;	/* Ausgabefunktion */
} InfoType_t;

struct Info_s {
	InfoType_t *type;
};

Info_t *Info_admin (Info_t *tg, const Info_t *src);
void Info_print (io_t *io, const Info_t *data);
void Info_subprint (io_t *io, const Info_t *data, const char *pfx);


/*	Informationsbasis: String
*/

typedef struct {
	InfoType_t *type;
	char *fmt;
} InfoString_t;

extern InfoType_t InfoType_cstr;
extern InfoType_t InfoType_mstr;
Info_t *InfoString_create (char *label);

#define	InfoString(label)	{ &InfoType_cstr, label }


/*	Informationsbasis: Ausgabefunktion
*/

typedef struct {
	InfoType_t *type;
	void (*func) (io_t *io);
} InfoFunc_t;

extern InfoType_t InfoType_func;

#define	InfoFunc(func)	{ &InfoType_func, func }


/*	Informationsbasis: Filelabel
*/

typedef struct {
	InfoType_t *type;
	char *name;	/* Filename */
	int start;	/* Startzeile (wird nicht angezeigt) */
	int end;	/* Endezeile (wird nicht angezeigt) */
} FileLabel_t;

extern InfoType_t InfoType_FileLabel;

#define	Label(name)	Label_ ## name
#define	UseLabel(x)	extern FileLabel_t Label(x)
#define	GlobalLabel(x)	FileLabel_t Label(x) = \
	{ &InfoType_FileLabel, __FILE__, __LINE__,
#define	LocalLabel(x)	static GlobalLabel(x)
#define	EndLabel	__LINE__ };


/*	Informationsknoten
*/

typedef struct InfoNode_s InfoNode_t;
typedef void (*PrintInfo_t) (io_t *io, InfoNode_t *info);
typedef void (*LoadInfo_t) (InfoNode_t *info);

struct InfoNode_s {
	PrintInfo_t func;	/* Ausgabefunktion */
	char *name;		/* Knotenname */
	char *label;		/* Knotenbezeichnung */
	void *par;		/* Ausgabeparameter */
	InfoNode_t *prev;	/* Vorgängerknoten */
	LoadInfo_t load;	/* Ladefunktion */
	vecbuf_t *list;		/* Unterknoten */
};

char *InfoNameToken (char **ptr);

InfoNode_t *NewInfo (InfoNode_t *base, char *name);
InfoNode_t *GetInfo (InfoNode_t *base, const char *name);
InfoNode_t *AddInfo (InfoNode_t *base, const char *name, char *label,
	PrintInfo_t func, void *par);

/*	Sonderzeichen
*/

#define	INFO_ESC	'\\'	/* Fluchtsymbol in Infodatenbanken */
#define	INFO_SEP	'/'	/* Trennzeichen im Informationspfad */
#define	INFO_INPUT	'<'	/* Eingabefile */
#define	INFO_PIPE	'|'	/* Eingabepipeline */

#define	INFO_KEY	'@'	/* Kennung eines Informationspfades */


/*	Ausgabedefinitionen
*/

int InfoName (io_t *io, InfoNode_t *base, InfoNode_t *info);
void PrintInfo (io_t *io, InfoNode_t *base, const char *name);
void DumpInfo (io_t *io, InfoNode_t *base, const char *name);

extern char *InfoPath;

void LoadInfo (InfoNode_t *base, const char *name);
void IOLoadInfo (InfoNode_t *base, io_t *io);


#endif	/* EFEU/Info.h */

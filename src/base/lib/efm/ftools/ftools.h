/*	Filehilfsprogramme
	(c) 1996 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.6

$Header	<EFEU/$1>
*/

#ifndef	EFEU_FTOOLS_H
#define	EFEU_FTOOLS_H	1

#include <EFEU/mstring.h>

#define	MSG_FTOOLS	"ftools"

#ifndef	SEEK_SET
#define	SEEK_SET	0
#define	SEEK_CUR	1
#define	SEEK_END	2
#endif

extern FILE *popen (const char *cmd, const char *type);
extern int pclose (FILE *stream);


/*	Dateien suchen
*/

typedef struct {
	char *path;	/* Bibliotheksname */
	char *name;	/* Filename */
	char *type;	/* Filetyp */
} fname_t;

extern fname_t *strtofn (const char *name);
extern char *fntostr (const fname_t *fname);
extern char *fsearch (const char *path, const char *pfx,
	const char *name, const char *ext);


/*	Datenfiles öffnen
*/

extern FILE *fileopen (const char *name, const char *mode);
extern FILE *tempopen (void);
extern FILE *findopen (const char *path, const char *pfx,
	const char *name, const char *ext, const char *mode);
extern int fileclose (FILE *file);
extern char *fileident (FILE *file);
extern void filemessage (FILE *file, const char *name, int num, int narg, ...);
extern void fileerror (FILE *file, const char *name, int num, int narg, ...);
extern void filenotice (char *name, FILE *file, int (*close) (FILE *file));
extern void closeall (void);

extern int filedebugflag;

/*	Binäre Ein/Ausgabe von Datenfiles
*/

extern size_t rfread (void *ptr, size_t size, size_t nitems, FILE *file);
extern size_t rfwrite (const void *ptr, size_t size, size_t nitems, FILE *file);

#if	REVBYTEORDER
#define	fread_compat	rfread
#define	fwrite_compat	rfwrite
#else
#define	fread_compat	fread
#define	fwrite_compat	fwrite
#endif

extern int fpeek (FILE *file);

extern unsigned get1byte (FILE *file);
extern unsigned get2byte (FILE *file);
extern unsigned get3byte (FILE *file);
extern unsigned get4byte (FILE *file);

extern void put1byte (unsigned val, FILE *file);
extern void put2byte (unsigned val, FILE *file);
extern void put3byte (unsigned val, FILE *file);
extern void put4byte (unsigned val, FILE *file);

extern size_t loadvec (FILE *file, void *ptr, size_t size, size_t dim);
extern size_t savevec (FILE *file, const void *ptr, size_t size, size_t dim);

extern char *loadstr (FILE *file);
extern void savestr (const char *str, FILE *file);

extern size_t dbread (FILE *file, void *dp, size_t rl, size_t size, size_t n);
extern size_t dbwrite (FILE *file, void *dp, size_t rl, size_t size, size_t n);

#endif	/* EFEU/ftools.h */

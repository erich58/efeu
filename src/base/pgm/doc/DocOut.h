/*	Dokumentausgabefilter
	(c) 1999 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5

	Version 0.4
*/

#ifndef	_EFEU_DocOut_h
#define	_EFEU_DocOut_h	1

#include <EFEU/io.h>

typedef struct {
	char *name;
	char *desc;
	io_t *(*filter) (io_t *io);
	char *post;
} DocType_t;

extern io_t *DocOut_sgml (io_t *io);
extern io_t *DocOut_test (io_t *io);
extern io_t *DocOut_syntex (io_t *io);
extern io_t *DocOut_mroff (io_t *io);
extern io_t *DocOut_term (io_t *io);

extern char *DocCmdName (int key);
extern char *DocEnvName (int key);

DocType_t *GetDocType (const char *name);
extern io_t *DocOut (DocType_t *type, const char *name);
extern io_t *DocFilter (DocType_t *type, io_t *io);
void DocOutInfo (const char *name, const char *desc);


#endif	/* EFEU/DocOut.h */

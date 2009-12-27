/*	Hilfsroutinen fuer Aggregatfile-Datenformat
	(c) 1992 Erich Fruehstueck
	A-1090 Wien, Waehringer Strasse 64/6
*/

#ifndef	OESTZ
#define	OESTZ	"oestz"

#include <EFEU/efmain.h>
#include <EFEU/efio.h>

#define	OESTZ_RECLEN	86	/* Satzlaenge eines Aggregatfiles */

void oestz_open (const char *name);
void oestz_close (void);
char *oestz_get (int type, int flag);
char *oestz_parse (const char *buf, int start, int end);

#define	oestz_cpy(p, start, end) mstrcpy(oestz_parse(p, start, end))

typedef struct {
	char *kennung;	/* Kennung */
	char *erhebung;	/* Erhebung */
	char *objekt;	/* Objektbeschreibung */
	size_t sdim;	/* Zahl der Selektoren */
	size_t gdim;	/* Zahl der Gegenstaende */
	size_t kdim;	/* Zahl der Kriterien */
	char **gname;	/* Bezeichner der Gegenstaende */
	char **kname;	/* Bezeichner der Kriterien */
} OESTZ_HDR;


OESTZ_HDR *oestz_hdr (void);
int oestz_TeXput (io_t *io, const char *buf);
char *oestz_TeXstr (const char *buf);
void oestz_names (OESTZ_HDR *hdr);

extern int TeXmode;

void oestz_msg (io_t *io, int num);
void oestz_doc (io_t *io, OESTZ_HDR *hdr, int flag);
void oestz_type (io_t *io, OESTZ_HDR *hdr, const char *name);
void oestz_datdef (io_t *io, OESTZ_HDR *hdr);
double *oestz_scale (OESTZ_HDR *hdr, const char *list);

void SetupOestz (void);


/*	Globale Variablen
*/

extern char *OestzHflag;
extern char *OestzSflag;
extern char *OestzGflag;
extern char *OestzKflag;

#endif	/* OESTZ */

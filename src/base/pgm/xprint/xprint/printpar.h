/*	Filterparameter zum Drucken
	(c) 1996 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

#ifndef	PRINTPAR_H
#define	PRINTPAR_H	1

#include <EFEU/efmain.h>
#include <EFEU/efio.h>

extern char *PCPath;

extern int TopMargin;
extern int LeftMargin;
extern int PageWidth;
extern int PageHeight;

extern int HeadHeight;
extern char *LeftHead;
extern char *CenterHead;
extern char *RightHead;

extern int FootHeight;
extern char *LeftFoot;
extern char *CenterFoot;
extern char *RightFoot;

extern int PageNumber;
extern char *NumberLine;
extern char *HeadLine;
extern char *FootLine;

extern int FirstPage;
extern int LastPage;

io_t *pc_setup(const char *name);
io_t *pc_filter(const char *name);
void headline(io_t *io, char *left, char *center, char *right);

extern int ulmode;
extern int fillmode;

extern char *pc_init;
extern char *pc_exit;
extern char *pc_newline;
extern char *pc_newpage;
extern char *pc_ulfmt;

typedef struct {
	int num;
	char *init;
	char *exit;
} fontdef_t;

fontdef_t *getfont(int num);
void loadfont(io_t *io, const char *delim);


/*	Kontrollfunktionen
*/

#define	PC_CTRL		('P' << 8)
#define	PC_FONT		(PC_CTRL|0x1)

/*	Druckmodus
*/

void mode_comment(io_t *in, io_t *out);
void mode_control(io_t *in, io_t *out);

#endif	/* PRINTPAR_H */

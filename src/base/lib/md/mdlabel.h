/*	Labelgenerierung
	(c) 1997 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

#ifndef MDLABEL_H
#define	MDLABEL_H	1

#include <EFEU/mdmat.h>

/*	Labelstruktur
*/

typedef struct MdLabel_s MdLabel_t;


struct MdLabel_s {
	MdLabel_t *next;
	char *name;	/* Name der Achse */
	size_t len;	/* Länge des Bezeichners */
	size_t dim;	/* Zahl der Muster */
	char **list;	/* Liste mit Muster */
	xtab_t *idx;	/* Tabelle mit Bezeichnern */
};


MdLabel_t *new_label (void);
void del_label (MdLabel_t *label);
MdLabel_t *set_label (const char *def);
MdLabel_t *init_label (const char *fmt, const char *def);

int save_label (io_t *tmp, MdLabel_t *label, char *p);
mdaxis_t *label2axis (MdLabel_t *label);


/*	Lesefilter
*/

#define	ICTRL_HEAD	1	/* Wechsel in Headermodus */
#define	ICTRL_TEXT	2	/* Wechsel in Textmode */
#define	ICTRL_NUM	3	/* Wechsel in numerischen Mode */
#define	ICTRL_IGNORE	4	/* Ignorieren von Zeichen im num. Mode */

io_t *rfilter (io_t *io);


#endif	/* MDLABEL_H */

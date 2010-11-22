#ifndef	EFEU_xml_h
#define	EFEU_xml_h	1

#include <EFEU/strbuf.h>
#include <EFEU/io.h>

typedef enum {
	xml_pi = 1,	/* Process instruction */
	xml_dtd,	/* Doctype-Deklarationen */

	xml_beg,	/* Element-Beginn */
	xml_att,	/* Attribut */
	xml_data,	/* Daten */
	xml_cdata,	/* Zeichendaten (kein Markup) */
	xml_end,	/* Element-Ende */

	xml_comm,	/* Kommentar */
	xml_err,	/* Fehlerkennung (spezielles Kommentar) */
} XMLType;

typedef struct XMLBuf XMLBuf;

typedef void *(*XMLAction) (XMLBuf *xml, const char *name,
	const char *data, void *par);

typedef struct {
	int (*put) (int c, void *par);
	void *par;
} XMLOutput;

struct XMLBuf {
	StrBuf sbuf;
	XMLAction action;
	void *par;
	XMLType type;
	int depth;
	int tag;
	int data;
};

#define	XML_DATA()	{ SB_DATA(0), NULL, NULL, 0, 0, 0 }
#define	XML_DECL(name)	XMLBuf name = XML_DATA()

XMLBuf *xml_create (XMLBuf *xml, XMLAction action, void *par);

void XMLBuf_start (XMLBuf *xml, const char *name);
int XMLBuf_next (XMLBuf *xml, int concat);
void XMLBuf_prev (XMLBuf *xml, int last);
int XMLBuf_last (XMLBuf *xml);
void *XMLBuf_action (XMLBuf *xml, XMLType which, int prev);
void *XMLBuf_close (XMLBuf *xml);
void *XMLBuf_parse (XMLBuf *xml, IO *io);
int XMLBuf_puts (XMLBuf *xml, const char *pfx, const char *str);

void *XMLBuf_pi (XMLBuf *xml, const char *name, const char *data);
void *XMLBuf_dtd (XMLBuf *xml, const char *name,
	const char *id, const char *decl);
void *XMLBuf_entry (XMLBuf *xml, const char *tag, const char *data);
void *XMLBuf_beg (XMLBuf *xml, const char *tag);
void *XMLBuf_att (XMLBuf *xml, const char *name, const char *data);
void *XMLBuf_data (XMLBuf *xml, XMLType type, const char *data);
void *XMLBuf_fmt (XMLBuf *xml, XMLType type, const char *fmt, ...);
void *XMLBuf_end (XMLBuf *xml);
void *XMLBuf_err (XMLBuf *xml, const char *fmt, ...);

void *xml_print (XMLBuf *xml, const char *name, const char *data, void *par);
void *xml_compact (XMLBuf *xml, const char *name, const char *data, void *par);
void *xml_list (XMLBuf *xml, const char *name, const char *data, void *par);
void *xml_dump (XMLBuf *xml, const char *name, const char *data, void *par);
void *xml_tlist (XMLBuf *xml, const char *name, const char *data, void *par);

#endif /* EFEU/xml.h */

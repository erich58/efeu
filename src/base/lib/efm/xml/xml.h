#ifndef	EFEU_xml_h
#define	EFEU_xml_h	1

#include <EFEU/strbuf.h>

typedef enum {
	xml_pi,		/* Process instruction */
	xml_comment,	/* Kommentar */
	xml_beg,
	xml_data,
	xml_end,
	xml_entry,	/* Kombination aus beg, data und end */
} XMLStatus;

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
	XMLStatus stat;
	int depth;
	int tag;
	int data;
};

#define	XML_DATA()	{ SB_DATA(0), NULL, NULL, 0, 0, 0 }
#define	XML_DECL(name)	XMLBuf name = XML_DATA()

XMLBuf *xml_create (XMLBuf *xml, XMLAction action, void *par);
void XMLBuf_start (XMLBuf *xml);
int XMLBuf_next (XMLBuf *xml);
void XMLBuf_prev (XMLBuf *xml, int last);
int XMLBuf_last (XMLBuf *xml);

void *XMLBuf_action (XMLBuf *xml, XMLStatus which);
void *XMLBuf_beg (XMLBuf *xml, const char *tag, const char *opt);
void *XMLBuf_data (XMLBuf *xml, const char *data);
void *XMLBuf_end (XMLBuf *xml);
void *XMLBuf_entry (XMLBuf *xml, const char *tag, const char *data);
void *XMLBuf_close (XMLBuf *xml);
void *XMLBuf_parse (XMLBuf *xml, int32_t (*get) (void *par), void *par);

void *xml_print (XMLBuf *xml, const char *name, const char *data, void *par);
void *xml_compact (XMLBuf *xml, const char *name, const char *data, void *par);
void *xml_list (XMLBuf *xml, const char *name, const char *data, void *par);
void *xml_dump (XMLBuf *xml, const char *name, const char *data, void *par);
void *xml_tlist (XMLBuf *xml, const char *name, const char *data, void *par);

void *xml_simple_print (XMLBuf *xml, const char *name,
		const char *data, void *par);
void *xml_simple_parse (XMLBuf *xml, int32_t (*get) (void *par), void *par);

#endif /* EFEU/xml.h */

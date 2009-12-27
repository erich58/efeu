/*	Auflisten von Selektionsdefinitionen
	(c) 1994 Erich Frühstück
*/

#include <EFEU/mdmat.h>
#include <EFEU/mdcount.h>


#define	MAX_COLUMNS	64	/* Max. Spaltenzahl */
#define	WIDTH		16	/* Breite für Zusammenfassung */

#define	LBL_CLASS	\
	":*:grouping classes" \
	":de:Selektionsklassen"

#define	HEAD	getmsg(MSG_MDMAT, 1, "#Name\tDIM\tDESCRIPTION\n")

char *MdClassListFormat = "$1[$3]\t$2\n";
char *MdClassPrintHead = "$1\t$2\n";
char *MdClassPrintEntry = "\t$1\t$2\n";
char *MdClassPrintFoot = NULL;
int MdClassPrintLimit = 20;

void MdClassList (io_t *io, MdClass_t *cdef)
{
	reg_cpy(1, cdef->name);
	reg_cpy(2, cdef->desc);
	reg_fmt(3, "%d", cdef->dim);
	io_psub(io, MdClassListFormat);
}

static void p_label(io_t *io, Label_t *label)
{
	reg_cpy(1, label->name);
	reg_cpy(2, label->desc);
	io_psub(io, MdClassPrintEntry);
}

void MdClassPrint (io_t *io, MdClass_t *cdef)
{
	int i;

	reg_cpy(1, cdef->name);
	reg_cpy(2, cdef->desc);
	reg_fmt(3, "%4d", cdef->dim);
	io_psub(io, MdClassPrintHead);

	if	(MdClassPrintLimit && cdef->dim > max(4, MdClassPrintLimit))
	{
		p_label(io, cdef->label);
		p_label(io, cdef->label + 1);
		reg_cpy(1, "...");
		reg_cpy(2, "...");
		io_psub(io, MdClassPrintEntry);
		p_label(io, cdef->label + cdef->dim - 2);
		p_label(io, cdef->label + cdef->dim - 1);
	}
	else
	{
		for (i = 0; i < cdef->dim; i++)
			p_label(io, cdef->label + i);
	}

	reg_cpy(1, cdef->name);
	reg_cpy(2, cdef->desc);
	io_psub(io, MdClassPrintFoot);
}

static char **list = NULL;
static size_t dim = 0;
static io_t *out = NULL;

static int show_short (MdClass_t *entry)
{
	MdClassList(out, entry);
	return 1;
}

static int show_long(MdClass_t *entry)
{
	if	(patselect(entry->name, list, dim))
		MdClassPrint(out, entry);

	return 1;
}

void MdShowClass(io_t *io, xtab_t *tab, const char *plist)
{
	out = io;

	if	(plist != NULL)
	{
		dim = strsplit(plist, ",;%s", &list);
		xwalk(tab, (visit_t) show_long);
		memfree(list);
	}
	else	xwalk(tab, (visit_t) show_short);
}


static void print_class(io_t *io, InfoNode_t *info)
{
	char *head = MdClassPrintHead;
	char *entry = MdClassPrintEntry;
	MdClassPrintHead = NULL;
	MdClassPrintEntry = "$1\t$2\n";
	MdClassPrint(iostd, info->par);
	MdClassPrintHead = head;
	MdClassPrintEntry = entry;
}

static InfoNode_t *class_root = NULL;

static int add_klass(MdClass_t *entry)
{
	AddInfo(class_root, entry->name, entry->desc,
		print_class, entry);
	return 1;
}

void MdClassInfo (InfoNode_t *info, xtab_t *tab)
{
	class_root = AddInfo(info, "class", LBL_CLASS, NULL, NULL);
	xwalk(tab, (visit_t) add_klass);
}

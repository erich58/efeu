/*	Makrodefinition
	(c) 1999 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5
*/

#include <EFEU/Document.h>
#include <EFEU/preproc.h>
#include <EFEU/cmdeval.h>
#include <EFEU/pconfig.h>
#include <ctype.h>

LocalLabel(def)/*
$1[tab]\\name[argdef]{defstr}
$1[tab]{namedef}[argdef]{defstr}

Der Befehl $1 definiert einen Makro mit Namen name und
der Makrodefinition defstr. In der zweiten Variante wird namedef
expandiert und kann beliebige Zeichen enthalten. Der Aufruf eines
solchen Makros ist mit \\call möglich.

Das optionale Argument tab bestimmt die Befehlstabelle,
in die der Befehl einzutragen ist. Ohne Angabe gilt die aktuelle Tabelle.

Der optionale Parameter argdef definiert die Parameterliste für den
Befehl. Er besteht aus einer Folge von Parameterkennungen (Buchstaben),
denen ein Widerholungsfaktor vorangestellt sein kann.
Die Parameterliste kann durch >>ungesättigte<< Makros am Ende
des Definitionsstrings verlängert werden.

Folgende Parameterkennungen sind zugelassen:
o	optionaler Parameter der Form [par]
a	notwendiger Parameter der Form {par}
x	optionaler Parameter der Form {par}
v	variable Parameterliste der Form {par}...{par}
n	Namensdefinition, wahlweise \\name oder {namedef}

Wird als Kennung ein Großbuchstabe verwendet, werden die Parameter
bereits vor Übergabe an die Makrodefinition ausgewertet.

Zusätzlich können noch Steuerzeichen enthalten sein. Sie haben
folgende Bedeutung:
*	Überliest weißen Zeichen
<space>	Überliest Leerzeichen und Tabulatoren

Der $1 - Befehl besitzt folgende Parameterkennung: "ONoa*"

Vor Aufruf eines Makros werden die Parameter in den Definitionsstring
eingemischt. Eine Paramterkennung besteht aus einem Gittersymbol, gefolgt
von einem Stern oder einer Nummer. Folgt nach dem Gittersymbol
ein anderes Zeichen, gilt dies nicht als Parameterkennung und wird
unverändert ausgegeben.

Die Parameter sind von 1 weg nummeriert. Der Parameter 0 verweist auf
den Makronamen. Ein Stern liefert eine Liste aller Parameter.
Ein Buchstabe nach der Parameterkennung wird als Formatierungszeichen
interpretiert. Anschließende Leerzeichen und Tabulatoren werden ignoriert.

Folgende Formatierungszeichen sind zugelassen:
s	Stringdarstellung zur Weitergabe an den Befehlsinterpreter,
	als Trennsymbol wird ein Komma verwendet.
o	Klammerung mit [], kein Trennsymbol
a	Klammerung mit {}, kein Trennsymbol

Das Trennsymbol kommt nur bei einer Parameterliste (Parameterkennung Stern)
zum Einsatz. Ohne Formatierungszeichen werden die Parameter unverändert
ausgegeben. Als Trennsymbol wird ein Leerzeichen verwendet.
*/EndLabel

static void def_label (DocMac_t *cmd, io_t *io)
{
	FileLabel_print(io, &Label(def));
}

static void def_eval (DocMac_t *cmd, Merge_t *merge, DocOut_t *out)
{
	DocMacTab_t *tab;
	char *name;
	DocMacPar_t mp;

	DocMacPar_init(&mp, cmd->name);
	DocMacPar_load(&mp, merge, cmd->par);
#if	0
	tab = DocMacStack_tab(DocMacPar_get(&mp, 1));
#else
	tab = NULL;
#endif
	name = DocMacPar_extract(&mp, 2);

	if	(tab && name)
	{
		char *desc = (merge->buf && merge->buf->pos) ?
			mstrncpy(merge->buf->data, merge->buf->pos) : NULL;
		DocMacTab_macro(tab, name, desc, DocMacPar_extract(&mp, 3),
			DocMacPar_extract(&mp, 4));
	}
	else	memfree(name);

	if	(merge->buf)
		sb_clear(merge->buf);

	DocMacPar_clean(&mp);
}

DocMac_t DocMac_def = DocMac_data("def",
	def_eval, def_label, NULL, "ONo a ");

DocMac_t DocMac_edef = DocMac_data("edef",
	def_eval, def_label, NULL, "ONo A ");

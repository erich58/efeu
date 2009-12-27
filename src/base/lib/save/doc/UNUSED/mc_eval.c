/*	Mischbefehl: Befehlsauswertung
	(c) 1999 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5
*/

#include <EFEU/Document.h>
#include <EFEU/preproc.h>
#include <EFEU/cmdeval.h>
#include <EFEU/pconfig.h>
#include <ctype.h>

LocalLabel(eval)/*
$1 {cmdlist}
$1 cmdlist

Der Befehl $1 wertet die Befehlsfolge cmdlist aus.
Die Befehlsfolge steht entweder in geschwungenen Klammern,
oder endet beim nächsten Zeilenvorschub.
Die Klammerung der Befehlsliste gilt nicht als Befehlsblock.

Wenn der Mischbefehl $1 zu Beginn einer Zeile steht,
gehört der Abschlußzeilenvorschub zur Befehlsliste,
ansonsten verbleibt er in der Eingabe.

Falls ein Ausdruck der Befehlszeile ein Ergebnis liefert
und nicht mit einem Strichpunkt abgeschlossen ist,
wird es zusammen mit dem Abschlußzeichen ausgegeben.

Mischbefehle in der Befehlsfolge werden nicht interpretiert.
Die Ausgabe der Befehlsausdrücke gilt als Eingabe für den Mischvorgang.
*/EndLabel

static void mf_cmdeval (DocMac_t *cmd, Merge_t *merge, DocOut_t *out)
{
	char *prompt;
	io_t *io;
	
	io = io_refer(merge->input);
#if	0
	prompt = io_prompt(io, "eval> ");
	merge->input = NULL;
	MergeEval_init(merge);

	if	((io_peek(io) == '{'))
	{
		io_getc(io);
		Merge_ioeval(merge, io_cmdeval(io, "}"));
		io_getc(io);
	}
	else	Merge_ioeval(merge, io_cmdeval(io, "%s"));

	MergeEval_exit(merge);
	io_close(merge->input);
	io_prompt(io, prompt);
#endif
	merge->input = io;
}

LocalLabel(newcmd)/*
$1[tab]\\name[argdef]{defstr}
$1[tab]{namedef}[argdef]{defstr}

Der Befehl $1 definiert einen Makro mit Namen name und
der Makrodefinition defstr. In der zweiten Variante wird namedef
expandiert und kann beliebige Zeichen enthalten. Der Aufruf eines
solchen Makros ist mit $1 möglich.

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

static void mf_newcmd (DocMac_t *cmd, Merge_t *merge, DocOut_t *out)
{
	DocMacTab_t *tab;
	char *name, *par, *cdef;

	name = Merge_arg(merge, '[', ']', 1);
/*
	tab = DocMacStack_tab(name);
*/
	tab = NULL;
	memfree(name);
/*
	name = Merge_cmdname(merge, ' ');
	par = Merge_arg(merge, '[', ']', 0);
	Merge_ignorespace(merge);
	io_protect(merge->input, 1);
	cdef = Merge_arg(merge, '{', '}', 0);
	io_protect(merge->input, 0);
*/
	/*
	Merge_ignorespace(merge);
	*/

	if	(tab && name)
	{
		char *desc = (merge->buf && merge->buf->pos) ?
			mstrncpy(merge->buf->data, merge->buf->pos) : NULL;
		DocMacTab_cmdeval(tab, name, desc, par, cdef);
	}
	else
	{
		memfree(name);
		memfree(par);
		memfree(cdef);
	}

	if	(merge->buf)
		sb_clear(merge->buf);
}

DocMac_t DocMac_cmdeval = DocMac_data("cmdeval",
	mf_cmdeval, MergeInfo_Label, NULL, &Label(eval));

DocMac_t DocMac_newcmd = DocMac_data("newcmd",
	mf_newcmd, MergeInfo_Label, NULL, &Label(newcmd));

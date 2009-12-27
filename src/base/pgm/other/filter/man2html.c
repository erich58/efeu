/*	Catman-Ausgabe in HTML-Datei konvertieren
	(c) 1998 Erich Fr�hst�ck
	A-1090 Wien, W�hringer Stra�e 64/6

	Version 1.0
*/

#include <stdio.h>
#include <ctype.h>

#define	HEAD	"<HTML><BODY><PRE>"
#define	FOOT	"</PRE></BODY></HTML>"

#define	ATT_RM	0
#define	ATT_IT	1
#define	ATT_BF	2

static char *att_beg[] = { "", "<EM>", "<STRONG>" };
static char *att_end[] = { "", "</EM>", "</STRONG>" };

static int cur_att = ATT_RM;
static int nl_count = 0;

static void put(int c, int att)
{
	if	(att != cur_att)
	{
		fputs(att_end[cur_att], stdout);
		cur_att = att;
		fputs(att_beg[cur_att], stdout);
	}

	if	(c == '\n')
	{
		if	(nl_count > 1)	return;

		nl_count++;
	}
	else	nl_count = 0;

	switch ((char) c)
	{
	case '<':	fputs("&lt;", stdout); break;
	case '>':	fputs("&gt;", stdout); break;
	case '&':	fputs("&amp;", stdout); break;
	case '�':	fputs("&Auml;", stdout); break;
	case '�':	fputs("&Ouml;", stdout); break;
	case '�':	fputs("&Uuml;", stdout); break;
	case '�':	fputs("&auml;", stdout); break;
	case '�':	fputs("&ouml;", stdout); break;
	case '�':	fputs("&uuml;", stdout); break;
	case '�':	fputs("&szlig;", stdout); break;
	default:	putc(c, stdout); break;
	}
}


/*	Hauptprogramm
*/

int main (int narg, char **arg)
{
	int last;
	int att;
	int c;

	cur_att = att = ATT_RM;
	fputs(HEAD, stdout);
	last = '\n';

	while ((c = getchar()) != EOF)
	{
		if	(c == 033)
		{
			c = getchar();

			if	(c == '[')
			{
				while ((c = getchar()) != EOF)
					if	(c >= 0x40)	break;
			}

			if	(c == EOF)	break;
			else			continue;
		}
		else if	(c == '\b')
		{
			c = getchar();

			if	(c == EOF)
			{
				break;
			}
			else if	(last == '_')
			{
				last = c;
				att = ATT_IT;
			}
			else if	(c == last)
			{
				att = ATT_BF;
			}
			else	last = c;

			continue;
		}

		put(last, att);
		last = c;
		att = ATT_RM;
	}

	put(last, att);
	fputs(FOOT, stdout);
	putc('\n', stdout);
	return 0;
}

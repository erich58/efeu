/*	Datei öffnen
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4

April 1999:
	Flexiblere Kompressionsverfahren.
	Automatische Einträge in Abhängigkeitslisten.
März 2000:
	Filename "-" wird wie Nullpointer behandelt.
	Filedeskriptorkennungen "&0", "&1", "&2".
*/

#include <EFEU/ftools.h>
#include <EFEU/procenv.h>
#include <EFEU/parsub.h>
#include <EFEU/patcmp.h>
#include <EFEU/MakeDepend.h>

/*	Zugriffsmodi
*/

#define	MODE_READ	0
#define	MODE_WRITE	1
#define	MODE_APPEND	2

static char *ftype[] = { "rb", "wb", "ab" };
static char *ptype[] = { "r", "w", "w" };


/*	Komressionskommandos
*/

static struct {
	int mode;
	char *ext;
	char *cmd;
} ztab[] = {
	{ 'r',  "gz",	"|gzip -cdf" },
	{ 'w',  "gz",	"|gzip -1 -c >" },
	{ 'a',  "gz",	"|gzip -1 -c >>" },
	{ 'r',  "bz2",	"|bzip2 -cd" },
	{ 'w',  "bz2",	"|bzip2 -c >" },
};

#define	zcmp(x,m,e)	((x).mode == (m) && strcmp((x).ext, (e)) == 0)

static char *zcmd (const char *name, const char *mode)
{
	char *ext;

	if	(strchr(mode, 'z') && (ext = strrchr(name, '.')))
	{
		int i;
		ext++;

		for (i = 0; i < tabsize(ztab); i++)
			if (zcmp(ztab[i], mode[0], ext))
				return mstrpaste(" ", ztab[i].cmd, name);
	}

	return NULL;
}


/*	Pipeline öffnen
*/

static FILE *open_pipe (char *name, int mode)
{
	FILE *file = popen(name + 1, ptype[mode]);
	
	if	(file == NULL)
	{
		message("popen", MSG_FTOOLS, 2, 2, name + 1, ptype[mode]);
		procexit(EXIT_FAILURE);
	}
	else	filenotice(name, file, pclose);

	return file;
}

/*	Datei öffnen
*/

static FILE *open_file (char *name, int mode)
{
	FILE *file = fopen(name, ftype[mode]);
	
	if	(file == NULL)
	{
		message("fopen", MSG_FTOOLS, 3, 2, name, ftype[mode]);
		procexit(EXIT_FAILURE);
	}
	else	filenotice(name, file, fclose);

	return file;
}


/*	Datei öffnen
*/

FILE *fileopen (const char *name, const char *mode)
{
	char *fname;
	int omode;

/*	Zugriffstype bestimmen
*/
	switch (mode[0])
	{
	case 'r':	omode = MODE_READ; break;
	case 'w':	omode = MODE_WRITE; break;
	case 'a':	omode = MODE_APPEND; break;
	default:
		message("fileopen", MSG_FTOOLS, 1, 2, name, mode);
		procexit(EXIT_FAILURE);
		return NULL;
	}

/*	Einträge in Abhängigkeitslisten
*/
	if	(strchr(mode, 'd'))
	{
		if	(omode != MODE_READ)
		{
			AddTarget(name);

			if	(MakeDepend)
				name = "/dev/null"; 
		}
		else	AddDepend(name);
	}

/*	Dateien öffnen
*/
	if	(name == NULL || (name[0] == '-' && name[1] == 0))
		return (omode == MODE_READ) ? stdin : stdout;

	if	(*name == '|')
		return open_pipe(mstrcpy(name), omode);

	if	(*name == '&')
	{
		switch (atoi(name + 1))
		{
		case  0:	return stdin;
		case  1:	return stdout;
		case  2:	return stderr;
		default:	break;
		}
	}

	if	((fname = zcmd(name, mode)))
		return open_pipe(fname, omode);
	
	return open_file(mstrcpy(name), omode);
}

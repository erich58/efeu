/*	Datenstrukturen mit Referenzzähler
	(c) 1995 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/


#include <EFEU/memalloc.h>
#include <EFEU/refdata.h>
#include <EFEU/io.h>
#include <EFEU/parsub.h>

#define	ERR	"fatal error in %s: use of a deleted object (%lx).\n"

#define	KEY_ALLOC	"alloc"
#define	KEY_REFER	"refer"
#define	KEY_DEREF	"deref"
#define	KEY_CLEAN	"clean"
#define	KEY_REUSE	"reuse"
#define	KEY_DEBUG	"debug"

/*	Hilfsfunktion zur Überprüfung eines Referenzobjektes.
	Bei gültigen Daten wird der Pointer geliefert.
	Die Funktion entfernt gleichzeitig eine const-Qualifikation
*/

static refdata_t *rd_check (const void *data, const char *name)
{
	refdata_t *rd = (refdata_t *) data;

	if	(rd && rd->reftype)
	{
		if	(rd->refcount == 0)
		{
			fprintf(stderr, ERR, name, (ulong_t) rd);
			exit(EXIT_FAILURE);
			return NULL;
		}
		else	return rd;
	}
	else	return NULL;
}

/*	Debug - Information
*/

static int debug_depth = 0;

static void debug_id (const refdata_t *rd, const char *cmd)
{
	io_nputc('\t', ioerr, debug_depth);
	io_printf(ioerr, "%s(", cmd);

	if	(rd->reftype->label)
		io_printf(ioerr, "%s ", rd->reftype->label);

	io_printf(ioerr, "%#lx)", (ulong_t) rd);
}

static void std_debug (const refdata_t *rd, const char *cmd)
{
	if	(rd->reftype && rd->reftype->debug)
	{
		char *ident = rd_ident(rd);

		debug_id(rd, cmd);

		if	(ident)
		{
			io_printf(ioerr, " %s", ident);
			memfree(ident);
		}

		io_putc('\n', ioerr);
	}
}

void rd_debug (const void *data, const char *fmt, ...)
{
	const refdata_t *rd = data;

	if	(rd && rd->reftype && rd->reftype->debug)
	{
		debug_id(rd, KEY_DEBUG);

		if	(fmt)
		{
			va_list args;

			va_start(args, fmt);
			io_putc(' ', ioerr);
			io_vprintf(ioerr, fmt, args);
			va_end(args);
		}
			
		io_putc('\n', ioerr);
	}
}

/*	Identifikation generieren
*/

char *rd_ident (const void *data)
{
	refdata_t *rd = rd_check(data, "rd_ident");
	return (rd && rd->reftype->ident) ? rd->reftype->ident(rd) : NULL;
}


/*	Datenobjekt genereieren
*/

void *rd_create (const reftype_t *type)
{
	if	(type && type->admin)
	{
		refdata_t *rd = type->admin(NULL, NULL);

		if	(rd)
		{
			rd->reftype = type;
			rd->refcount = 1;
			std_debug(rd, KEY_ALLOC);
		}

		return rd;
	}

	return NULL;
}


/*	Referenzzähler erhöhen
*/

void *rd_refer (const void *data)
{
	refdata_t *rd = rd_check(data, "rd_refer");

	if	(rd != NULL)
	{
		std_debug(rd, KEY_REFER);
		rd->refcount++;
	}

	return rd;
}

/*	Referenzzähler verringern/Daten freigeben
*/

void rd_deref (void *data)
{
	refdata_t *rd = rd_check(data, "rd_deref");

	if	(rd == NULL)	return;

	if	(rd->refcount > 1)
	{
		rd->refcount--;
		std_debug(rd, KEY_DEREF);
	}
	else
	{
		std_debug(rd, KEY_CLEAN);
		rd->refcount = 0;

		if	(rd->reftype && rd->reftype->admin)
		{
			debug_depth++;
			rd->reftype->admin(data, NULL);
			debug_depth--;
		}
	}
}


#if	0
void *rd_copy (const void *data)
{
	refdata_t *rd = rd_check(data, "rd_copy");

	if	(rd->refcount > 1 && rd->reftype->admin)
	{
		refdata_t *tg = rd->reftype->admin(NULL, rd);

		if	(tg != rd)
		{
			if	(tg)
			{
				tg->reftype = rd->reftype;
				tg->refcount = 1;
				std_debug(tg, KEY_ALLOC);
			}

			rd_deref(rd);
			return tg;
		}
	}

	std_debug(rd, KEY_REUSE);
	return rd;
}

void *rd_admin (void *tptr, const void *sptr)
{
	refdata_t *tg = rd_check(tptr, "rd_admin");
	refdata_t *src = rd_check(sptr, "rd_admin");

	if	(tg->reftype != src->reftype)
	{
		message("rd_admin", MSG_EFM, 11, 0);
		return tg;
	}

	if	(tg->reftype->admin)
	{
		tg->reftype->admin(tg, src);
		rd_deref(src);
	}
}
#endif

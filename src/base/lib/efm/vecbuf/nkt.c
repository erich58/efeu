/*
Tabelle mit Namensschlüssel

$Copyright (C) 2001 Erich Frühstück
This file is part of EFEU.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this library; see the file COPYING.Library.
If not, write to the Free Software Foundation, Inc.,
59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
*/

#include <EFEU/nkt.h>
#include <EFEU/mstring.h>

#define	NKT_BSIZE	125

static char *nkt_rt_ident (const void *ptr)
{
	const NameKeyTab *nkt = ptr;
	return msprintf("%s %s[%d]", nkt->reftype->label,
		nkt->name, nkt->tab.used);
}

static void nkt_rt_clean (void *ptr)
{
	NameKeyTab *nkt = ptr;
	nkt_clean(ptr);
	memfree(nkt->name);
	memfree(nkt);
}

/*
Die externe Variable |$1| definiert den Referenztyp für
Suchtabellen mit Namensschlüssel.
*/

RefType nkt_reftype = REFTYPE_INIT("NameKeyTab",
	nkt_rt_ident, nkt_rt_clean);

/*
Die Funktion |$1| generiert eine neue Schlüsseltabelle mit Namen |name|,
Blockgröße |bsize| und Löschfunktion |ckean|.
*/

NameKeyTab *nkt_create (const char *name, size_t bsize,
	void (*clean) (void *data))
{
	NameKeyTab *nkt = memalloc(sizeof(NameKeyTab));
	nkt->name = mstrcpy(name);
	nkt->clean = clean;
	vb_init(&nkt->tab, bsize ? bsize : NKT_BSIZE, sizeof(NameKeyEntry));
	return rd_init(&nkt_reftype, nkt);
}

/*
Die Funktion |$1| löscht alle Einträge del Schlüsseltabelle <nkt>.
*/

void nkt_clean (NameKeyTab *nkt)
{
	if	(!nkt)	return;

	if	(nkt->clean)
	{
		NameKeyEntry *entry = nkt->tab.data;

		while (nkt->tab.used)
		{
			nkt->clean(entry->data);
			nkt->tab.used--;
			entry++;
		}
	}
	else	nkt->tab.used = 0;
}

static int nkt_cmp (const void *pa, const void *pb)
{
	const NameKeyEntry *a = pa;
	const NameKeyEntry *b = pb;
	return mstrcmp(a->name, b->name);
}

/*
Die Funktion |$1| erweitert die Schlüsseltabelle <nkt> um den
Eintrag <name> mit den Daten <data>. Die Funktion liefert 1
falls ein alter Eintrag ersetzt wurde und 0 sonst.
*/

int nkt_insert (NameKeyTab *nkt, const char *name, void *data)
{
	if	(nkt)
	{
		NameKeyEntry entry, *ptr;
		entry.name = name;
		entry.data = data;
		ptr = vb_search(&nkt->tab, &entry, nkt_cmp, VB_REPLACE);

		if	(ptr && nkt->clean && ptr->data)
			nkt->clean(ptr->data);

		return ptr ? 1 : 0;
	}
	else	return 0;
}

/*
Die Funktion |$1| löscht den Eintrag <name> aus der Schlüsseltabelle <nkt>.
Die Funktion liefert 1 falls ein Eintrag gelöscht wurde und 0 falls
kein entsprechender Eintrag existierte.
*/

int nkt_delete (NameKeyTab *nkt, const char *name)
{
	if	(nkt)
	{
		NameKeyEntry entry, *ptr;
		entry.name = name;
		entry.data = NULL;
		ptr = vb_search(&nkt->tab, &entry, nkt_cmp, VB_DELETE);

		if	(ptr && nkt->clean && ptr->data)
			nkt->clean(ptr->data);

		return ptr ? 1 : 0;
	}
	else	return 0;
}

/*
Die Funktion |$1| ruft den Eintrag <name> aus der Schlüsseltabelle
<nkt> ab. Falls der Eintrag nicht gefunden wurde,
liefert die Funktion <defval>.
*/

void *nkt_fetch (NameKeyTab *nkt, const char *name, void *defval)
{
	if	(nkt)
	{
		NameKeyEntry entry, *ptr;
		entry.name = name;
		entry.data = NULL;
		ptr = vb_search(&nkt->tab, &entry, nkt_cmp, VB_SEARCH);
		return ptr ? ptr->data : defval;
	}
	else	return defval;
}

/*
Die Funktion |$1| ruft für jeden Eintrag der Suchtabelle die
Funktion <visit> mit dem Eintragsnamen <name>, dem Datenpointer <data>
und dem Parameterwert <par>, der beim Aufruf der Funktion übergeben wurde.
Die Funktion |nkt_walk| stoppt die Verarbeitung, sobald die Funktion <visit>
einen Wert ungleich 0 liefert und gibt diesen als Rückgabewert zurück.
*/

int nkt_walk (NameKeyTab *nkt,
	int (*visit) (const char *name, void *data, void *par), void *par)
{
	NameKeyEntry *ptr;
	size_t n;
	int result;

	if	(nkt == NULL || visit == NULL)	return 0;

	for (result = 0, ptr = nkt->tab.data, n = nkt->tab.used; n-- > 0; ptr++)
		if ((result = visit(ptr->name, ptr->data, par))) break;

	return result;
}

/*
Die Funktion |$1| arbeitet wie |nkt_walk|, jedoch werden die
Einträge in umgekehrter Reihenfolge abgearbeitet.
*/

int nkt_rwalk (NameKeyTab *nkt,
	int (*visit) (const char *name, void *data, void *par), void *par)
{
	NameKeyEntry *ptr;
	size_t n;
	int result;

	if	(nkt == NULL || visit == NULL)	return 0;

	for (result = 0, ptr = nkt->tab.data, n = nkt->tab.used; n-- > 0; )
		if ((result = visit(ptr[n].name, ptr[n].data, par))) break;

	return result;
}

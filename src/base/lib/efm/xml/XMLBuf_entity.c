#include <EFEU/xml.h>
#include <EFEU/mstring.h>

static struct {
	char *name;
	char *repl;
} tab[] = {
	{ "amp", "&" },
	{ "apos", "'" },
	{ "gt", ">" },
	{ "lt", "<" },
	{ "quot", "\"" },

	{ "Auml", "\xC3\x84" },
	{ "Ouml", "\xC3\x96" },
	{ "Uuml", "\xC3\x9C" },
	{ "auml", "\xC3\xA4" },
	{ "ouml", "\xC3\xB6" },
	{ "uuml", "\xC3\xBC" },
	{ "szlig", "\xC3\x9F" },

	{ "euro", "\xE2\x82\xAC" },
};

#define	DIM	(sizeof tab / sizeof tab[0])

char *XMLBuf_entity (XMLBuf *xml, const char *name)
{
	int i;

	if	(*name == '#')
	{
		static unsigned char buf[8];
		int val;

		name++;

		if	(*name == 'x' || *name == 'X')
			val = strtoul(name + 1, NULL, 16);
		else	val = strtoul(name, NULL, 10);

		val = nputucs(val, buf, sizeof buf);
		buf[val] = 0;
		return (char *) buf;
	}

	for (i = 0; i < DIM; i++)
		if (strcmp(tab[i].name, name) == 0)
			return tab[i].repl;

	return NULL;
}

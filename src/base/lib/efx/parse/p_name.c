/*	Namen lesen
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/object.h>
#include <EFEU/Op.h>
#include <ctype.h>

#define	NAME_KEY	"operator"

char *Parse_name (io_t *io)
{
	void *name;

	if	((name = io_getname(io)) == NULL)
	{
		io_error(io, MSG_EFMAIN, 81, 0);
		return NULL;
	}

	if	(strcmp(name, NAME_KEY) == 0)
	{
		memfree(name);

		if	(io_eat(io, " \t") == '"')
		{
			io_scan(io, SCAN_STR, &name);
		}
		else	name = io_mgets(io, "%s");
	}

	return name;
}

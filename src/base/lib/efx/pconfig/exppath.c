/*	Pfadnamen expandieren
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/pconfig.h>
#include <EFEU/strbuf.h>
#include <EFEU/extension.h>
#include <ctype.h>

#if	HAS_GETPWNAM
#include <pwd.h>
#endif

char *ExpandPath (const char *name)
{
	strbuf_t *sb;

	if	(name == NULL)	return NULL;

	sb = new_strbuf(0);

	while (*name != 0)
	{
		if	(*name == '~')
		{
#if	HAS_GETPWNAM
			if	(isalpha(name[1]))
			{
				int save;
				struct passwd *pw;

				save = sb_getpos(sb);

				while (*(++name) != 0)
				{
					if	(*name == '/')
					{
						break;
					}
					else	sb_putc(*name, sb);
				}

				sb_putc(0, sb);
				pw = getpwnam((char *) sb->data + save);
				sb_setpos(sb, save);
				sb_sync(sb);

				if	(pw)
				{
					sb_puts(pw->pw_dir, sb);
				}

				continue;
			}
#endif
			sb_puts(getenv("HOME"), sb);
		}
		else	sb_putc(*name, sb);

		name++;
	}
	
	return sb2str(sb);
}

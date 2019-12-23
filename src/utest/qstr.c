#include <EFEU/UnitTest.h>

const char *UnitTest_qstr(const char *str)
{
	static char buf[1024];
	size_t n = 0;

	if	(str)
	{
		buf[n++] = '"';

		for (; *str; ++str)
		{
			if	(n + 6 >= sizeof buf)
			{
				buf[n++] = '.';
				buf[n++] = '.';
				buf[n++] = '.';
				break;
			}

			switch (*str)
			{
			case '"':	buf[n++] = '\\'; buf[n++] = '"'; break;
			case '\\':	buf[n++] = '\\'; buf[n++] = '\\'; break;
			case '\t':	buf[n++] = '\\'; buf[n++] = 't'; break;
			case '\n':	buf[n++] = '\\'; buf[n++] = 'n'; break;
			default:	buf[n++] = *str; break;
			}
		}

		buf[n++] = '"';
		buf[n] = 0;
	}
	else
	{
		strcpy(buf, "(null)");
	}

	return buf;
}

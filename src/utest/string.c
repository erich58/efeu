#include <EFEU/UnitTest.h>
#include <string.h>
#include <sys/types.h>
#include <regex.h>

int UnitTest_strcmp(const char *a, const char *b)
{
	if	(a == b)	return 0;
	if	(a == NULL)	return -1;
	if	(b == NULL)	return 1;

	return strcmp(a, b);
}

int UnitTest_match(const char *a, const char *expr)
{
	if	(expr == NULL)
		return (a == NULL);

	if	(a == NULL)
		return 0;

	char err_buf[256];
	regex_t buf;
	int n;
	n = regcomp(&buf, expr, REG_EXTENDED|REG_NOSUB);

	if	(n != 0)
	{
		regerror(n, &buf, err_buf, sizeof err_buf);
		fprintf(stderr, "#ERROR\t%s\n", err_buf);
		regfree(&buf);
		return 0;
	}

	n = regexec(&buf, a, 0, NULL, 0);
	regfree(&buf);
	return n == 0;
}

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
			case '\f':	buf[n++] = '\\'; buf[n++] = 'f'; break;
			case '\b':	buf[n++] = '\\'; buf[n++] = 'b'; break;
			case '\v':	buf[n++] = '\\'; buf[n++] = 'v'; break;
			case '\r':	buf[n++] = '\\'; buf[n++] = 'r'; break;
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

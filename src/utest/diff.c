#include <EFEU/UnitTest.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <regex.h>
#include <errno.h>

int UnitTest_diff(const char *file, int line,
	const char *s1, const char *s2, const char *x1, const char *x2)
{
	if	(!s1 && !s2)
		return UnitTest_result(file, line, 1, "%s == %s", x1, x2);

	int r = UnitTest_strcmp(s1, s2) == 0;

	if	(!x1 || *x1 == '"')
		x1 = "[expr]";

	const char *p;

	if	(r)
	{
		STRING_DECL(buf);
		p = UnitTest_qsplit(&buf, s1);
		UnitTest_result(file, line, r, "%s == %s", x1, p);

		while ((p = UnitTest_next(&buf)))
			UnitTest_info(file, line, "+", "\t%s", p);

		UnitTest_free(&buf);
		return r;
	}

	if	(!(s1 && *s1) && !(s2 && *s2))
	{
		UnitTest_result(file, line, 0, "%s == %s", x1,
			UnitTest_qstr(s2));
		UnitTest_info(file, line, "BECAUSE", "%s -> %s", x1,
			UnitTest_qstr(s1));
		return 0;
	}

	UnitTest_mkfile(".actual",  s1 ? s1 : "");
	UnitTest_mkfile(".desired",  s2 ? s2 : "");
	system(UnitTest_tpl("diff -U1 @/.desired @/.actual > @/.diff"));
	s1 = UnitTest_content(".diff");
	UnitTest_mkfile(".actual", NULL);
	UnitTest_mkfile(".desired", NULL);
	UnitTest_mkfile(".diff", NULL);
	UnitTest_result(file, line, 0, x1);

	STRING_DECL(buf);
	STRING_DECL(lbuf);
	UnitTest_lsplit(&buf, s1);
	UnitTest_next(&buf);
	UnitTest_next(&buf);
	const char *pfx = "=";
	int color = 1;

	while ((p = UnitTest_next(&buf)))
	{
		switch (*p)
		{
		case '+':
			color = 3;
			pfx = "?+"; break;
		case '-':
			color = 2;
			pfx = "!-"; break;
		case '@':
			color = 0;
			pfx = "@"; break;
		case '\\':
			break;
		default:
			color = 1;
			pfx = "*="; break;
			break;
		}

		if	(color)
		{
			UnitTest_clear(&lbuf);
			UnitTest_puts(UnitTest_color(color), &lbuf);
			UnitTest_puts(p, &lbuf);
			UnitTest_puts(UnitTest_color(0), &lbuf);
			p = UnitTest_str(&lbuf);
		}

		UnitTest_info(NULL, 0, pfx, "%s", p);
	}

	UnitTest_free(&buf);
	UnitTest_free(&lbuf);
	return 0;
}

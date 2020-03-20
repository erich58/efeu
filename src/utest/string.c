#include <EFEU/UnitTest.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <regex.h>

#define	BSIZE	64

static UnitTestString *UnitTest_realloc(UnitTestString *buf, size_t n)
{
	while (buf && buf->size + n > buf->capacity)
	{
		buf->capacity += buf->capacity ? buf->capacity : BSIZE;
		buf->data = realloc(buf->data, buf->capacity);
	}

	return buf;
}

void UnitTest_clear(UnitTestString *buf)
{
	if	(buf)
		buf->size = 0;
}

void UnitTest_free(UnitTestString *buf)
{
	if	(buf)
	{
		buf->size = 0;
		buf->capacity = 0;
		buf->data = realloc(buf->data, buf->capacity);
	}
}

void UnitTest_putc(int c, UnitTestString *buf)
{
	if	(UnitTest_realloc(buf, 1))
		buf->data[buf->size++] = c;
}

void UnitTest_puts(const char *str, UnitTestString *buf)
{
	if	(str)
	{
		while (*str)
			UnitTest_putc(*str++, buf);
	}
}

const char *UnitTest_cstr(UnitTestString *buf)
{
	if	(UnitTest_realloc(buf, 1))
	{
		buf->data[buf->size] = 0;
		return buf->data;
	}

	return "";
}

int UnitTest_strcmp(const char *a, const char *b)
{
	if	(a == b)	return 0;
	if	(a == NULL)	return -1;
	if	(b == NULL)	return 1;

	return strcmp(a, b);
}

int UnitTest_match(const char *a, const char *expr)
{
	int neg = 0;

	if	(expr == NULL)
		return (a == NULL);

	if	(*expr == '!')
	{
		neg = 1;
		++expr;
	}

	if	(a == NULL)
		return neg;

	char err_buf[256];
	regex_t buf;
	int n;
	n = regcomp(&buf, expr, REG_EXTENDED|REG_NOSUB);

	if	(n != 0)
	{
		regerror(n, &buf, err_buf, sizeof err_buf);
		UnitTest_error("%s", err_buf);
		regfree(&buf);
		return neg;
	}

	n = regexec(&buf, a, 0, NULL, 0);
	regfree(&buf);
	return n == 0 ? !neg : neg;
}

void UnitTest_qputs(const char *str, int delim, UnitTestString *buf)
{
	if	(str && buf)
	{
		UnitTest_putc(delim, buf);

		for (; *str; ++str)
		{
			unsigned char c = *str;

			switch (c)
			{
			case '\\':	UnitTest_puts("\\\\", buf); break;
			case '\t':	UnitTest_puts("\\t", buf); break;
			case '\n':	UnitTest_puts("\\n", buf); break;
			case '\f':	UnitTest_puts("\\f", buf); break;
			case '\b':	UnitTest_puts("\\b", buf); break;
			case '\v':	UnitTest_puts("\\v", buf); break;
			case '\r':	UnitTest_puts("\\r", buf); break;
			default:
				if	(c == delim)
				{
					UnitTest_putc('\\', buf);
					UnitTest_putc(delim, buf);
				}
				else if	(c < ' ' || c >= 127)
				{
					UnitTest_putc('\\', buf);
					UnitTest_putc('0' + (c >> 6), buf);
					UnitTest_putc('0' + (c >> 3) % 8, buf);
					UnitTest_putc('0' + c % 8, buf);
				}
				else
				{
					UnitTest_putc(c, buf);
				}

				break;
			}
		}

		UnitTest_putc(delim, buf);
	}
	else
	{
		UnitTest_puts("(null)", buf);
	}
}

const char *UnitTest_qstr(const char *str)
{
	static STRING_DECL(buf);
	UnitTest_clear(&buf);
	UnitTest_qputs(str, '"', &buf);
	return UnitTest_cstr(&buf);
}

void UnitTest_vprintf(UnitTestString *s, const char *fmt, va_list list)
{
	if	(!s || !fmt)
		return;

	va_list l;
	va_copy(l, list);
	ssize_t n = vsnprintf(NULL, 0, fmt, l);
	va_end(l);

	if	(n > 0)
	{
		UnitTest_realloc(s, n + 1);
		vsnprintf(s->data + s->size, s->capacity - s->size, fmt, list);
		s->size += n;
	}
}

void UnitTest_printf(UnitTestString *s, const char *fmt, ...)
{
	va_list list;
	va_start(list, fmt);
	UnitTest_vprintf(s, fmt, list);
	va_end(list);
}

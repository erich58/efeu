#include <EFEU/UnitTest.h>
#include <EFEU/object.h>
#include <EFEU/printobj.h>
#include <EFEU/parsedef.h>

static STRBUF(buf, 256);

static IO *IN(const char *str)
{
	static IO *io = NULL;
	io_close(io);
	io = io_cstr(str);
	return io;
}

static const char *list2str(EfiObjList *list)
{
	sb_trunc(&buf);
	sb_printf(&buf, "%d:", ObjListLen(list));

	while (list != NULL)
	{
		sb_putc(' ', &buf);
		sb_puts(list->obj ? list->obj->type->name : "NULL", &buf);
		list = list->next;
	}

	return sb_nul(&buf);
}

TEST_DECL(parse)
{
	EfiObj *a, *b;
	EXPECT(Parse_fmt(IN("(3){int x; }"), "( T ) C", &a, &b));
	EXPECT(a->type == &Type_int);
	EXPECT(b->type == &Type_block);
	UnrefObj(a);
	UnrefObj(b);

	EfiObjList *list;
	EXPECT(Parse_fmt(IN("(3, 2.5, 'c')"), "(L)", &list));
	EXPECT_STR(list2str(list), "3: int double char");
	DelObjList(list);

	EXPECT(Parse_fmt(IN("()"), "(L)", &list));
	EXPECT_STR(list2str(list), "0:");
	DelObjList(list);

	EXPECT(Parse_fmt(IN("(3, 5)"), "(A L)", &a, &list));
	EXPECT(a->type == &Type_int);
	EXPECT_STR(list2str(list), "1: int");
	UnrefObj(a);
	DelObjList(list);

	EXPECT(Parse_fmt(IN("(x)"), "(A L)", &a, &list));
	EXPECT(a->type == &Type_name);
	EXPECT_STR(list2str(list), "0:");
	UnrefObj(a);
	DelObjList(list);

	char *name;
	EXPECT(Parse_fmt(IN("(x, 1)"), "(N L)", &name, &list));
	EXPECT_STR(name, "x");
	EXPECT_STR(list2str(list), "1: int");
	memfree(name);
	DelObjList(list);
}

TEST_SUITE
{
	SetupStd();
	TEST_RUN(parse);
	IN(NULL);
	sb_free(&buf);
}

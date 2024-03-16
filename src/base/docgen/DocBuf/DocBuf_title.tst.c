#include <EFEU/UnitTest.h>
#include "DocBuf.h"

TEST_SUITE
{
	STRBUF(buf, 64);
	DocBuf doc;
	DocBuf_init(&doc);
	DocBuf_title(&doc, &buf, "Cmd-Name");
	EXPECT_STR(doc.var[VAR_TITLE], NULL);
	EXPECT_STR(doc.var[VAR_COPYRIGHT], NULL);

	// Delimited by paragraph
	sb_puts("Title\n$1\n\nLast Line", &buf);
	DocBuf_init(&doc);
	DocBuf_title(&doc, &buf, "Cmd-Name");
	EXPECT_STR(doc.var[VAR_TITLE], "Title\nCmd-Name\n");
	EXPECT_STR(doc.var[VAR_COPYRIGHT], NULL);

	// Delimited by section
	sb_puts("Title\n$copyright Name\nLast Line", &buf);
	DocBuf_init(&doc);
	DocBuf_title(&doc, &buf, "Cmd-Name");
	EXPECT_STR(doc.var[VAR_TITLE], "Title\n");
	EXPECT_STR(doc.var[VAR_COPYRIGHT], "Name");

	// Set by section
	sb_puts("First Line\n$title Title Line\nLast Line", &buf);
	DocBuf_init(&doc);
	DocBuf_title(&doc, &buf, "Cmd-Name");
	EXPECT_STR(doc.var[VAR_TITLE], "Title Line");
	EXPECT_STR(doc.var[VAR_COPYRIGHT], NULL);
}

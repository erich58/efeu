#include <EFEU/UnitTest.h>
#include <EFEU/MD5Context.h>

#define	TEST_DATA	"1234567890ABCDEFGHIJKLMNOPQRSTUVMXYZ\n"
#define	TEST_DIGEST	"ca8929e5a0725353d32f906293d62246"
#define	EMPTY_DIGEST	"d41d8cd98f00b204e9800998ecf8427e"

static const char *md5(unsigned char digest[16])
{
	static char buf[34];
	int i;

	for (i = 0; i < 16; i++)
		sprintf(buf + 2 * i, "%02x", digest[i]);

	return buf;
}

TEST_SUITE
{
	unsigned char buf[] = TEST_DATA;
	unsigned char digest[16];
	MD5Context ctx;

	MD5Context_init (&ctx);
	MD5Context_update (&ctx, buf, sizeof buf - 1);
	MD5Context_update (&ctx, buf, sizeof buf - 1);
	MD5Context_update (&ctx, buf, sizeof buf - 1);
	MD5Context_update (&ctx, buf, sizeof buf - 1);
	MD5Context_update (&ctx, buf, sizeof buf - 1);
	MD5Context_final (digest, &ctx);
	PRINT_STR(md5(digest));
	EXPECT_STR(md5(digest), TEST_DIGEST);

	MD5Context_init (&ctx);
	MD5Context_update (&ctx, NULL, 0);
	MD5Context_final (digest, &ctx);
	EXPECT_STR(md5(digest), EMPTY_DIGEST);
}

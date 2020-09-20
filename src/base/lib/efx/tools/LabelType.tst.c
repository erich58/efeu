#include <EFEU/UnitTest.h>
#include <EFEU/object.h>
#include <EFEU/stdtype.h>

TEST_SUITE
{
	SetupStd();

	Label label[] = {
		{"first", "First label" },
		{"second", "Second label" },
	};

	EfiType *type = LabelType("TestLabel", label, 2);
	EXPECT(type != NULL);
	EXPECT(EnumKeyCode(type, "first") == 0);
	EXPECT(EnumKeyCode(type, "second") == 1);
	EXPECT(EnumKeyCode(type, "undefined") == 0);
}

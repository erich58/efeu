#include <EFEU/UnitTest.h>
#include <EFEU/stack.h>

TEST_SUITE
{
	Stack *stack = NULL;
	pushstack(&stack, "a");
	pushstack(&stack, "b");
	pushstack(&stack, "c");
	EXPECT_STREQ(popstack(&stack, NULL), "c");
	EXPECT_STREQ(popstack(&stack, NULL), "b");
	EXPECT_STREQ(popstack(&stack, NULL), "a");
	EXPECT_TRUE(stack == NULL);
	EXPECT_STREQ(popstack(&stack, "x"), "x");
}

#include <EFEU/Resource.h>

int main (int argc, char **argv)
{
	char *fmt, *label;

	ParseCommand(&argc, argv);
	fmt = GetResource("Format", "%s");
	label = GetResource("Label", "Hello World!");

	printf(fmt, label);
	putchar('\n');
	exit(EXIT_SUCCESS);
}

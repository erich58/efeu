#include <EFEU/Resource.h>

int main (int argc, char **argv)
{
	char *fmt, *label;

	SetVersion("vhello2.c 1.0");
	ParseCommand(&argc, argv);
	fmt = GetResource("Format", "%s");
	label = GetResource("Label", "Hello World!");

	printf(fmt, label);
	putchar('\n');
	exit(EXIT_SUCCESS);
}

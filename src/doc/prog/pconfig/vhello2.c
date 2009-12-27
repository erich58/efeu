#include <EFEU/Resource.h>

int main (int argc, char **argv)
{
	char *fmt, *label;

	SetVersion("$Id: vhello2.c,v 1.2 2003-01-17 11:25:02 ef Exp $");
	ParseCommand(&argc, argv);
	fmt = GetResource("Format", "%s");
	label = GetResource("Label", "Hello World!");

	printf(fmt, label);
	putchar('\n');
	exit(EXIT_SUCCESS);
}

/*
Schablonendatei für <"Hello World"> Beispielprogramm
*/

config src

#include <macro.ms>
#include <function.ms>

StdInclude(src, "stdio");

GlobalFunction("int main", "int argc, char **argv", string !
printf("Hello World!\n");
return 0;
!);

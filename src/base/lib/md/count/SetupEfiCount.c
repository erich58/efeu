/*	Z�hldefinitionen initialisieren
	$Copyright (C) 2007 Erich Fr�hst�ck
	A-3423 St.Andr�/W�rdern, Wildenhaggasse 38
*/

#include <EFEU/EfiCount.h>
#include <EFEU/Info.h>

void SetupEfiCount (void)
{
	static int init_done = 0;
	InfoNode *info;

	if	(init_done)	return;

	init_done = 1;
	AddEfiPar(NULL, &EfiCount_record);
	info = GetInfo(NULL, NULL);
	EfiCountInfo(info);
}

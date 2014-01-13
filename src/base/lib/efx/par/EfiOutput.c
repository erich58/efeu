/*	EFEU-Ausgabedefinitionen
	$Copyright (C) 2006 Erich Frühstück
	A-3423 St.Andrä/Wördern, Wildenhaggasse 38
*/

#include <EFEU/EfiOutput.h>

EfiPar EfiPar_output = { "output", "data printing method" };

void SetupEfiOutput (void)
{
	static int setup_done = 0;

	if	(setup_done)	return;

	setup_done = 1;
	AddEfiPar(NULL, &EfiOutput_binary);
}

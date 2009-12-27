/*	EFEU-Eingabedefinitionen
	$Copyright (C) 2006 Erich Fr�hst�ck
	A-3423 St.Andr�/W�rdern, Wildenhaggasse 38
*/

#include <EFEU/EfiInput.h>

EfiPar EfiPar_input = { "input", "data reading method" };

void SetupEfiInput (void)
{
	static int setup_done = 0;

	if	(setup_done)	return;

	setup_done = 1;
	AddEfiPar(NULL, &EfiInput_binary);
	AddEfiPar(NULL, &EfiInput_plain);
	AddEfiPar(NULL, &EfiInput_label);
}

/*	Binäre Eingabefunktion
	$Copyright (C) 2006 Erich Frühstück
	A-3423 St.Andrä/Wördern, Wildenhaggasse 38
*/

#include <EFEU/EfiInput.h>

static void *open_binary (IO *io, const EfiType *type,
	void *par, const char *opt, const char *arg)
{
	return io;
}

static int read_binary (EfiType *type, void *data, void *par)
{
	if	(io_peek(par) == EOF)
		return 0;

	return ReadData(type, data, par) != 0;
}

EfiInput EfiInput_binary = {
	EPC_DATA(&EfiPar_input, NULL, "binary", "binary input", NULL),
	open_binary, read_binary, NULL,
};

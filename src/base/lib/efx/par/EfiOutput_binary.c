/*	Binäre Ausgabefunktion
	$Copyright (C) 2006 Erich Frühstück
	A-3423 St.Andrä/Wördern, Wildenhaggasse 38
*/

#include <EFEU/EfiOutput.h>

static void *open_binary (IO *io, const EfiType *type,
	void *par, const char *opt, const char *arg)
{
	return io;
}

static size_t write_binary (EfiType *type, void *data, void *par)
{
	return WriteData(type, data, par);
}

EfiOutput EfiOutput_binary = {
	EPC_DATA(&EfiPar_output, NULL, "binary", "binary output", NULL),
	open_binary, write_binary, NULL,
};

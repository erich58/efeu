/*	Regressionsparameter ausgeben
	(c) 1997 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 1.0
*/

#include <EFEU/object.h>
#include <Math/TimeSeries.h>

#define KFMT	"%-9s %15.6g %15.6g %15.6g\n"

int PrintOLSKoef (io_t *io, OLSKoef_t *koef)
{
	return io_printf(io, KFMT, koef->name,
		koef->val, koef->se, koef->se ? koef->val / koef->se : 0.);
}

int PrintOLSPar (io_t *io, OLSPar_t *par)
{
	int i, n;
	char *p1, *p2;

	if	(par == NULL)	return 0;

	p1 = TimeIndex2str(par->base, 0);
	p2 = TimeIndex2str(par->base, par->nr - 1);
	n  = io_printf(io, "Schätzzeitraum:%10s\t%25s\n", p1, p2);
	memfree(p1);
	memfree(p2);
	n += io_printf(io, "Beobachtungen: %10d\t", par->nr);
	n += io_printf(io, "Freiheitsgrade:%10d\n", par->df);
	n += io_printf(io, "R**2:          %10.6f\t", par->r2);
	n += io_printf(io, "RBAR**2:       %10.6f\n", par->rbar2);
	n += io_printf(io, "Durbin Watson: %10.6f\t", par->dw);
	n += io_printf(io, "SEE:           %10.3f\n", par->see);
	n += io_printf(io, "%-9s %15s %15s %15s\n", "Variable",
		"Koeffizient", "Standardfehler", "T-Statistic");

	for (i = 0; i < par->dim; i++)
		n += PrintOLSKoef(io, par->koef + i);

	return n;
}

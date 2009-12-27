/*	Zeitreihe in Datenmatrix konvertieren
	(c) 1997 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 1.0
*/

#include <EFEU/object.h>
#include <Math/TimeSeries.h>

static char *type_name(int type)
{
	switch (type)
	{
	case TS_YEAR:	return "jahr";
	case TS_QUART:	return "quart";
	case TS_MONTH:	return "monat";
	case TS_WEEK:	return "woche";
	case TS_DAY:	return "tag";
	default:	break;
	}

	return "index";
}

mdmat_t *TimeSeries2mdmat (TimeSeries_t *ts)
{
	mdmat_t *md;
	double *ptr;
	int i;

	if	(ts == NULL)	return NULL;

/*	Datenmatrix generieren
*/
	md = new_mdmat();
	md->type = &Type_double;
	md->title = mstrcpy(ts->name);

/*	Achse initialisieren
*/
	md->axis = new_axis(ts->dim);
	md->axis->name = mstrcpy(type_name(ts->base.type));

	for (i = 0; i < ts->dim; i++)
		md->axis->idx[i].name = TimeIndex2str(ts->base, i);

/*	Datenwerte speichern
*/
	md->size = md_size(md->axis, md->type->size);
	md->data = memalloc(md->size);
	ptr = md->data;

	for (i = 0; i < ts->dim; i++)
		ptr[i] = ts->data[i];

	return md;
}

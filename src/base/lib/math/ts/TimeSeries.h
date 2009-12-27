/*
:*:time series
:de:Zeitreihenanalyse

$Header <Math/$1>

$Copyright (C) 1997 Erich Frühstück
This file is part of Math.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this library; see the file COPYING.Library.
If not, write to the Free Software Foundation, Inc.,
59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
*/

#ifndef	Math_TimeSeries_h
#define	Math_TimeSeries_h	1

#include <EFEU/config.h>
#include <EFEU/refdata.h>
#include <EFEU/object.h>
#include <EFEU/mdmat.h>

#define	MSG_TS	"TimeSeries"

/*	Zeitindex
*/

#define	TS_INDEX	'#'	/* Indexnummer */
#define	TS_YEAR		'y'	/* Jahresdaten */
#define	TS_QUART	'q'	/* Quartalsdaten */
#define	TS_MONTH	'm'	/* Monatsdaten */
#define	TS_WEEK		'w'	/* Wochenzähler */
#define	TS_DAY		'd'	/* Tageszähler */
#define	TS_HOUR		'H'	/* Stunden */
#define	TS_MIN		'M'	/* Minuten */
#define	TS_SEC		'S'	/* Sekunden */

typedef struct {
	unsigned type : 8;	/* Zeiteinheit */
	unsigned value : 24;	/* Zeitwert */
} TimeIndex;


int tindex_type (const char *name);

TimeIndex tindex_create (int type, int value);
TimeIndex str2TimeIndex (const char *str, char **ptr);
int tindex_print (IO *io, TimeIndex idx, int offset);
char *TimeIndex2str (TimeIndex idx, int offset);
TimeIndex tindex_conv (TimeIndex idx, int type, int pos);
int tindex_diff (TimeIndex a, TimeIndex b);
int tindex_length (TimeIndex idx);
int tindex_year (TimeIndex idx);
int tindex_quart (TimeIndex idx);
int tindex_month (TimeIndex idx);
int tindex_day (TimeIndex idx);
int tindex_floor (TimeIndex idx);
int tindex_ceil (TimeIndex idx);

double TimeIndex2dbl (TimeIndex idx, int offset, int pos);


/*	Zeitreihenstruktur
*/

typedef struct {
	REFVAR;		/* Referenzvariablen */
	char *name;	/* Name der Zeitreihe */
	double *data;	/* Datenwerte */
	TimeIndex base;	/* Zeitbasis */
	size_t dim;	/* Zeitreihenlänge */
	size_t size;	/* Speicherfeldgröße */
	char *fmt;	/* Darstellungsformat */
} TimeSeries;

extern RefType TimeSeries_reftype;

TimeSeries *ts_create (const char *name, TimeIndex idx, size_t dim);
void ts_expand (TimeSeries *ts, size_t dim);
void ts_sync (TimeSeries *ts, TimeIndex idx, size_t dim, int offset);

TimeSeries *ts_copy (const char *name, const TimeSeries *ts,
	int a, int b);

TimeSeries *ts_read (IO *io);
int ts_write (IO *io, const TimeSeries *ts);
int ts_print (IO *io, const TimeSeries *ts, const char *fmt);

TimeSeries *ts_diff (TimeSeries *ts, int lag, EfiVirFunc *func);
void ts_cumulate (TimeSeries *ts, TimeSeries *base, EfiVirFunc *func);

TimeSeries *ts_ma (TimeSeries *ts, int n, int adjust);
void ts_ima (TimeSeries *ts, TimeSeries *base, int adjust);

TimeSeries *ts_convert (const char *name, TimeSeries *ts,
	int type, const char *flags);


double ts_value (TimeSeries *ts, TimeIndex idx, double defval);
mdmat *TimeSeries2mdmat (TimeSeries *ts);


/*	Lineare Regression
*/

typedef struct {
	char *name;	/* Variablenname */
	double val;	/* Datenwert */
	double se;	/* Standardfehler */
} OLSCoeff;


typedef struct {
	REFVAR;		/* Referenzvariablen */
	int nr;		/* Zahl der Beobachtungen */
	int df;		/* Zahl der Freiheitsgrade */
	double r2;	/* R-Quadrat */
	double rbar2;	/* Adjustiertes R-Quadrat */
	double dw;	/* Durbin Watson */
	double see;	/* SEE */
	size_t dim;		/* Zahl der exogenen Variablen */
	OLSCoeff *koef;	/* Koeffizientenvektor */
	TimeSeries **exogen;	/* Exogene Variablen */
	TimeSeries *res;	/* Residuen */
	TimeIndex base;	/* Basis der Schätzperiode */
	TimeIndex first;	/* Erster Index für Projektionen */
	TimeIndex last;	/* Letzter Index für Projektionen */
} OLSPar;

OLSPar *OLSPar_create (size_t dim);

void Func_OLS (EfiFunc *func, void *rval, void **arg);
void Func_OLSProj (EfiFunc *func, void *rval, void **arg);

int OLSCoeff_print (IO *io, OLSCoeff *koef);
int OLSPar_print (IO *io, OLSPar *par);

extern RefType OLSPar_reftype;


/*	Schnittstelle zum Interpreter
*/

extern EfiType Type_TimeSeries;
extern EfiType Type_TimeIndex;
extern EfiType Type_OLSCoeff;
extern EfiType Type_OLSPar;

#define	Val_TimeIndex(x)	((TimeIndex *) x)[0]
#define	Val_TimeSeries(x)	((TimeSeries **) x)[0]
#define	Val_OLSCoeff(x)		((OLSCoeff *) x)[0]
#define	Val_OLSPar(x)		((OLSPar **) x)[0]

void SetupTimeSeries (void);

void CmdSetup_TimeIndex (void);
void CmdSetup_TimeSeries (void);
void CmdSetup_OLSCoeff (void);
void CmdSetup_OLSPar (void);

void ts_assign (EfiVirFunc *fptr, TimeSeries *ts,
	EfiType *type2, void *arg2);
TimeSeries *ts_func (EfiVirFunc *fptr, TimeSeries *ts);
TimeSeries *ts_term (EfiVirFunc *fptr,
	EfiType *type1, void *arg1,
	EfiType *type2, void *arg2);

TimeSeries *ts_seasonal (const char *name, const TimeSeries *ts, int p);

TimeSeries *ExpSmoothing (const char *name, const TimeSeries *ts,
	double alpha, double sw, int k);
double ExpSmoothingError (const TimeSeries *ts, double alpha, double sw);
double ExpSmoothingInitial (const TimeSeries *ts, double alpha);

#endif	/* Math/TimeSeries.h */

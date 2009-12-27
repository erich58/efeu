/*	Zeitreihenanalyse
	(c) 1997 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 1.0
*/

#ifndef	Math_TimeSeries_h
#define	Math_TimeSeries_h	1

#include <EFEU/config.h>
#include <EFEU/types.h>
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

typedef struct {
	unsigned type : 8;	/* Zeiteinheit */
	unsigned value : 24;	/* Zeitwert */
} TimeIndex_t;


int TimeIndexType (const char *name);

TimeIndex_t TimeIndex (int type, int value);
TimeIndex_t str2TimeIndex (const char *str);
char *TimeIndex2str (TimeIndex_t idx, int offset);
TimeIndex_t TimeIndexKonv (TimeIndex_t idx, int type, int pos);
int DiffTimeIndex (TimeIndex_t a, TimeIndex_t b);
int TimeIndexLength (TimeIndex_t idx);
int TimeIndexYear (TimeIndex_t idx);
int TimeIndexMonth (TimeIndex_t idx);
int TimeIndexDay (TimeIndex_t idx);
int TimeIndexFloor (TimeIndex_t idx);
int TimeIndexCeil (TimeIndex_t idx);

double TimeIndex2dbl (TimeIndex_t idx, int offset, int pos);


/*	Zeitreihenstruktur
*/

typedef struct {
	REFVAR;			/* Referenzvariablen */
	char *name;		/* Name der Zeitreihe */
	double *data;		/* Datenwerte */
	TimeIndex_t base;	/* Zeitbasis */
	size_t dim;		/* Zeitreihenlänge */
	size_t size;		/* Speicherfeldgröße */
	char *fmt;		/* Darstellungsformat */
} TimeSeries_t;

extern reftype_t TimeSeries_reftype;

TimeSeries_t *NewTimeSeries (const char *name, TimeIndex_t idx, size_t dim);
void ExpandTimeSeries (TimeSeries_t *ts, size_t dim);
void SyncTimeSeries (TimeSeries_t *ts, TimeIndex_t idx, size_t dim);

TimeSeries_t *CopyTimeSeries (const char *name, const TimeSeries_t *ts,
	int a, int b);
int PrintTimeSeries (io_t *io, const TimeSeries_t *ts, const char *fmt);

TimeSeries_t *DiffTimeSeries (TimeSeries_t *ts, int lag, VirFunc_t *func);
void CumulateTimeSeries (TimeSeries_t *ts, TimeSeries_t *base, VirFunc_t *func);

TimeSeries_t *KonvTimeSeries (const char *name, TimeSeries_t *ts,
	int type, const char *flags);

double TimeSeriesValue (TimeSeries_t *ts, TimeIndex_t idx, double defval);
mdmat_t *TimeSeries2mdmat (TimeSeries_t *ts);


/*	Lineare Regression
*/

typedef struct {
	char *name;	/* Variablenname */
	double val;	/* Datenwert */
	double se;	/* Standardfehler */
} OLSKoef_t;


typedef struct {
	REFVAR;		/* Referenzvariablen */
	int nr;		/* Zahl der Beobachtungen */
	int df;		/* Zahl der Freiheitsgrade */
	double r2;	/* R-Quadrat */
	double rbar2;	/* Adjustiertes R-Quadrat */
	double dw;	/* Durbin Watson */
	double see;	/* SEE */
	size_t dim;		/* Zahl der exogenen Variablen */
	OLSKoef_t *koef;	/* Koeffizientenvektor */
	TimeSeries_t **exogen;	/* Exogene Variablen */
	TimeSeries_t *res;	/* Residuen */
	TimeIndex_t base;	/* Basis der Schätzperiode */
	TimeIndex_t first;	/* Erster Index für Projektionen */
	TimeIndex_t last;	/* Letzter Index für Projektionen */
} OLSPar_t;

OLSPar_t *NewOLSPar (size_t dim);

void Func_OLS (Func_t *func, void *rval, void **arg);
void Func_OLSProj (Func_t *func, void *rval, void **arg);

int PrintOLSKoef (io_t *io, OLSKoef_t *koef);
int PrintOLSPar (io_t *io, OLSPar_t *par);

extern reftype_t OLSPar_reftype;


/*	Schnittstelle zum Interpreter
*/

extern Type_t Type_TimeSeries;
extern Type_t Type_TimeIndex;
extern Type_t Type_OLSKoef;
extern Type_t Type_OLSPar;

#define	Val_TimeIndex(x)	((TimeIndex_t *) x)[0]
#define	Val_TimeSeries(x)	((TimeSeries_t **) x)[0]
#define	Val_OLSKoef(x)		((OLSKoef_t *) x)[0]
#define	Val_OLSPar(x)		((OLSPar_t **) x)[0]

void SetupTimeSeries (void);

void CmdSetup_TimeIndex (void);
void CmdSetup_TimeSeries (void);
void CmdSetup_OLSKoef (void);
void CmdSetup_OLSPar (void);

void AssignTimeSeries (VirFunc_t *fptr, TimeSeries_t *ts,
	Type_t *type2, void *arg2);
TimeSeries_t *TimeSeriesFunc (VirFunc_t *fptr, TimeSeries_t *ts);
TimeSeries_t *TimeSeriesTerm (VirFunc_t *fptr,
	Type_t *type1, void *arg1,
	Type_t *type2, void *arg2);

#endif	/* Math_TimeSeries_h */

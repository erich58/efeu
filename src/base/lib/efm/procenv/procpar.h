/*	Prozessparameter
	(c) 1998 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5

	Version 0.6

$Header	<EFEU/$1>
*/

#ifndef	EFEU_PROCPAR_H
#define	EFEU_PROCPAR_H	1

#include <EFEU/procenv.h>

#define	PPAR_TYPE_ENV		1	/* Umgebungsvariablen */
#define	PPAR_TYPE_KEY		2	/* Schlüsseloption */
#define	PPAR_TYPE_OPT		3	/* Standardoption */
#define	PPAR_TYPE_ARG		4	/* Argument */
#define	PPAR_TYPE_PAT		5	/* Muster */
#define	PPAR_TYPE_BRK		6	/* Abbruch der Argumentprüfung */ 

#define	PPAR_FLAG_STDARG	1	/* Standardargument */
#define	PPAR_FLAG_OPTARG	2	/* optionales Argument */
#define	PPAR_FLAG_HIDDEN	3	/* optionales Argument */

typedef struct {
	int type;	/* Parametertype */
	int flag;	/* Flags */
	char *key;	/* Kennung */
	char *name; 	/* Argumentname */
} ppar_key_t;

typedef struct {
	char *id;	/* Identifikationskennung */
	char *eval;	/* Auswertungsfunktion */
	char *cmd;	/* Initialisierungsfunktion */
	char *desc;	/* Beschreibungstext */
} ppar_def_t;


#endif	/* EFEU/procpar.h */

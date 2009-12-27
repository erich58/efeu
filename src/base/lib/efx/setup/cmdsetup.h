/*	Befehlsinterpreter initialisieren
	(c) 1996 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#ifndef	EFEU_CMDSETUP_H
#define	EFEU_CMDSETUP_H	1

#include <EFEU/efmain.h>
#include <EFEU/efio.h>
#include <EFEU/object.h>
#include <EFEU/stdtype.h>

void CmdSetup_const(void);	/* Konstanten */
void CmdSetup_base(void);	/* Basistypen */
void CmdSetup_int(void);	/* Ganzzahlwerte */
void CmdSetup_long(void);	/* Ganzzahlwerte */
void CmdSetup_double(void);	/* Gleitkommawerte */
void CmdSetup_str(void);	/* Zeichen und Strings */
void CmdSetup_io(void);		/* Ein-, Ausgabestrukturen */
void CmdSetup_print(void);	/* Ausgabefunktionen */
void CmdSetup_reg(void);	/* Stringregister */
void CmdSetup_obj(void);	/* Universielle Operatoren */
void CmdSetup_pctrl(void);	/* Programmsteuerung */
void CmdSetup_test(void);	/* Testfunktionen */
void CmdSetup_konv(void);	/* Konvertierungen */
void CmdSetup_func(void);	/* Funktionen */
void CmdSetup_op(void);		/* Operatoren einrichten */
void CmdSetup_info(void);	/* Informationsdatenbank */
void CmdSetup_strbuf(void);	/* Stringbuffer */
void CmdSetup_date(void);	/* Datum- und Zeit */
void CmdSetup_unix(void);	/* UNIX-Systemstrukturen */

#endif	/* EFEU_CMDSETUP_H */

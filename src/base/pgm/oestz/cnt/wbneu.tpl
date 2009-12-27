/*	Auszählen der Wohnungen
	(c) 1998 Erich Frühstück
	A-1090 Wien, Waehringer Strasse 64/6
*/

config src 

#include <function.ms>
#include <macro.ms>
#include <classdef.ms>

Include(src, string !
agf.h
wbneu.h
!);

ClassDef class = ClassDef("ushort_t *data");

class.simple("ges", "Gesamtheit", "0", 1, "GES", "Insgesamt");

class.simple("jahr", "Jahr", "data[WBNEU_A10] - 1980", 17,
	"$(n + 1980:%d)", "$(n + 1980:%d)");

class.limit("ANZ", NULL, "data[WBNEU_1]", 10, "ANZ[$(n)]");
class.limit("NFL", NULL, "data[WBNEU_11]", 140, "NFL[$(n)]");
class.limit("WRZ", NULL, "data[WBNEU_16]", 10, "WRZ[$(n)]");
class.limit("nfl", NULL, "data[WBNEU_11]/data[WBNEU_1]", 140, "nfl[$(n)]");
class.limit("nfl10", NULL, "data[WBNEU_11]/(10 * data[WBNEU_1])", 50, "$(10*n)");
class.limit("nfl50", NULL, "data[WBNEU_11]/(50 * data[WBNEU_1])", 20, "$(50*n)");
class.limit("wrz", NULL, "data[WBNEU_16]/data[WBNEU_1]", 10, "wrz[$(n)]");

class.test("wg", "Wohnungsgröße",
	"int nfl = data[WBNEU_11] / data[WBNEU_1];", string !
nfl<45	G1
nfl<60	G2
nfl<90	G3
nfl<130	G4
*	G5
!);

class.classfunc("kat", "Kategorie", string !
if	(data[WBNEU_S46] == 2)	return data[WBNEU_S37] == 2 ? 3 : 2;
if	(data[WBNEU_S82] == 1)	return 0;
if	(data[WBNEU_S82] == 3)	return 0;
else				return 1;
!, string !
Ka	Kategorie A
Kb	Kategorie B
Kc	Kategorie C
Kd	Kategorie D
!);

class.switch("bvroh", NULL, "data[WBNEU_T27]", string !
3	Rmw
1	Reb
2	Rew
4	Rdn
*	Rso
!);

class.classfunc("besitz", "Besitz", string !
switch (data[WBNEU_T27])
{
case  1:	return 1;
case  2:	return 2;
case  3:
	switch (data[WBNEU_C47])
	{
	case  2:	return 4;	/* Gemeinnützige Wohnbauvereinigung */
	case  3:	return 3;	/* Gebietskörperschaft */
	default:	return 0;
	}
case  4:	return 5;
default:	return 6;
}
!, string !
Wmp
Web
Wew
Wmg
Wmf
Wdn
Wso
!);

class.flag("bew", "Bewilligte Wohnungen",
	"data[WBNEU_B93] == 1", "BEW");
class.flag("fertig", "Fertiggestellte Wohnungen",
	"data[WBNEU_B93] == 2", "FERTIG");
class.flag("wien", "Wiener Wohnungen",
	"data[WBNEU_B00] == 9", "WIEN");

class.simple("gembez", NULL,
	"S07_to_gembez(data[WBNEU_S07])", 24, "B$(n:%02d)");
	
class.simple("bezirk", NULL, "data[WBNEU_S07]-99", 23, "B$(n+1:%02d)");

class.switch("rv", "Rechtsverhältnis", "data[WBNEU_T27]", string !
1	Reb
2	Rew
3	Rmw
4	Rdn
5	Rso
!);

class.simple("A10", NULL, "data[WBNEU_A10] - 1980", 20, "A10[$(n+1)]");
class.simple("B00", NULL, "data[WBNEU_B00] - 1", 9, "B00[$(n+1)]");
class.simple("B93", NULL, "data[WBNEU_B93] - 1", 2, "B93[$(n+1)]");
class.simple("C41", NULL, "data[WBNEU_C41] - 1", 2400, "C41[$(n+1)]");
class.simple("C47", NULL, "data[WBNEU_C47] - 1", 4, "C47[$(n+1)]");
class.simple("D04", NULL, "data[WBNEU_D04] - 1", 5, "D04[$(n+1)]");
class.simple("E00", NULL, "data[WBNEU_E00]", 6, "E00[$(n)]");
class.simple("Q62", NULL, "data[WBNEU_Q62] - 1", 2, "Q62[$(n+1)]");
class.simple("S07", NULL, "data[WBNEU_S07] - 1", 121, "S07[$(n+1)]");
class.simple("S37", NULL, "data[WBNEU_S37] - 1", 2, "S37[$(n+1)]");
class.simple("S46", NULL, "data[WBNEU_S46] - 1", 2, "S46[$(n+1)]");
class.simple("S82", NULL, "data[WBNEU_S82]", 5, "S82[$(n)]");
class.simple("T18", NULL, "data[WBNEU_T18] - 1", 8, "T18[$(n+1)]");
class.simple("T27", NULL, "data[WBNEU_T27] - 1", 5, "T27[$(n+1)]");
class.simple("T45", NULL, "data[WBNEU_T45] - 1", 2, "T45[$(n+1)]");

class.setup("sl_wbneu");

Function f = Function("void c_wbneu", "ushort_t *data");
f.line("wohnungen = data[WBNEU_1];");
f.line("personen = 0;");
f.write(true);

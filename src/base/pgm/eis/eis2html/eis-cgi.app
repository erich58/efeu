/*	EFEU-Informationssystem
	(c) 2000 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5
*/

Ident =	"CGI-Schnittstelle zu eis"
Version = "2.0"
Copyright = "(c) 2000 Erich Frühstück"

fmt_head = string !
<h3 ALIGN=center>EIS - Informationssystem</h3>
!;

fmt_failed = "Anfrage konnte nicht beantwortet werden\n\n";

fmt_foot = string !
<hr>
<a HREF="/local/eis.html">
<IMG HEIGHT="24" WIDTH="24" SRC="/images/eis.gif" alt="EIS">
</a> &copy; Erich Frühstück
!;

PgmVaArg("[file|cmd=val] [node=val]", NULL, "Knotendefinition")

/*	EFEU-Informationssystem
	(c) 2000 Erich Fr�hst�ck
	A-3423 St.Andr�/W�rdern, S�dtirolergasse 17-21/5
*/

Ident =	"CGI-Schnittstelle zu eis"
Version = "2.0"
Copyright = "(c) 2000 Erich Fr�hst�ck"

fmt_head = string !
<h3 ALIGN=center>EIS - Informationssystem</h3>
!;

fmt_failed = "Anfrage konnte nicht beantwortet werden\n\n";

fmt_foot = string !
<hr>
<a HREF="/local/eis.html">
<IMG HEIGHT="24" WIDTH="24" SRC="/images/eis.gif" alt="EIS">
</a> &copy; Erich Fr�hst�ck
!;

PgmVaArg("[file|cmd=val] [node=val]", NULL, "Knotendefinition")

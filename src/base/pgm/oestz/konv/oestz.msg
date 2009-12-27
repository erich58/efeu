/*	Formatdefinitionen für Aggregatfile-Datenformat
	(c) 1995 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 2.0
*/

1	$!: Unvollständige letzte Zeile $1\n
2	$!: $1: Falscher Satztyp, $2 erwartet.\n
3	$!: Datensektor nicht gefunden.\n

111	/*\t$1\n\tDatei wurde mit $! generiert.\n*/\n
11	%\t$1\n\
%\tDokument wurde mit $! generiert.\n\
\\documentstyle[A4,german,twocolumn]{report}\n\
\\begin{document}\n\
\\begin{titlepage}\n\\centering\n\
\\vspace*{3cm}\n\
\\Large \\bf\n\
Aggregatfile: $0\\\\ \n\
$1\\\\ \n\
\\vspace*{1cm}\n\
\\huge \\bf\n\
$2\\\\ \n\
\\vfill\n\
\\normalsize \\rm\n\
Dokument wurde mit \\verb|$!| generiert\\\\ \n\
\\end{titlepage}\n\
\\tableofcontents\n
12	\n\\chapter{Selektoren}\n
13	\n\\chapter{Gegenst\"ande}\n
14	\n\\chapter{Kriterien}\n
15	\\end{document}\n

121	G_$1\t:$3($4)\ta\t2\t$2\n
122	$1\t:$3($4)\ta\t2\t$2\n
21	\\section*{$1\~--\~$2}\n\
\\addcontentsline{toc}{section}{\\protect\\numberline{$1}$2}\n
/*
22	\\begin{raggedright}\n
23	\\end{raggedright}\n\n
*/
22	\n
23	\n

31	\\begin{list}{0}{%\n\
\\def\\makelabel#1{\\hfill#1\~$$\\big|$$\~}\n\
\\labelsep 0pt \\labelwidth 3em \\leftmargin \\labelwidth\n\
\\parsep -0.2ex \\itemsep0pt\n\
}\n
32	\\item[$1] $2
33	\n
34	\\footnote{{\\spaceskip 2pt plus 4pt minus 1pt\n
35	}}\n
36	\\end{list}\n

41	\\medskip\n{\\bf Zahl der Auspr\"agungen: $1.}\n
42	
43	
44	
45	
46	

141	$1\n
142	
143	
144	
145	
146	

51	/*\tHeaderdatei fuer Aggregatfile-Datenbank:\n\
\t$1\n\t$2\n\
\tDatei wurde mit $! generiert.\n\
*/\n\n\
#ifndef\t$0_DEFINED\n\
#define\t$0_DEFINED\t1\n\
\n

52	\
#define\t$0_GDIM\t$1\t/* Zahl der Gegenstaende */\n\
#define\t$0_KDIM\t$2\t/* Zahl der Kriterien */\n\n\
#define\t$0_DIM\t($0_GDIM + $0_KDIM)\n\n
53	#define $0_$(#1:%-3s)\t$2\n
54	\n#endif /* $0_DEFINED */\n

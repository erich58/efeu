# :*: documentation depending on installed modules
# :de: Von installierten Modulen abhängige Dokumentationen
##Depends: manuals xdoc

include foreach.smh
include usage.smh

mf_usage <<!
PDF-Version der Dokumente generieren

Aufruf: make doc
!

mf_rule -d doc "" "efeucheck pdfdoc && \$(MAKE) doc.pdf"

make_pdf()
{
	mf_dir -A doc.pdf -p $TOP/$2/pdf
	mf_srclist $1 $TOP/$2/ps
	foreach -A doc.pdf -p $TOP/$2/pdf -s ps.gz \
		'mf_rule -a $tg.pdf $src "zcat \$< | ps2pdf - \$@"'
}

#make_pdf doc_main doc
make_pdf doc_de doc/de
make_pdf doc_en doc/en

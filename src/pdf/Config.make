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
	foreach -A doc.pdf -p $1 -s ps.gz \
		'mf_rule -a $tg.pdf $src "zcat \$< | ps2pdf - \$@"' $2
}

make_pdf $TOP/doc/pdf $TOP/doc/ps
make_pdf $TOP/doc/de/pdf $TOP/doc/de/ps
make_pdf $TOP/doc/en/pdf $TOP/doc/en/ps

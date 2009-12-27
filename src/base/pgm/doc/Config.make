include efeu.smh
include readline.smh

mf_dir CFGDIR=$TOP/lib/efeudoc
mf_dir BIN=$TOP/bin

efeu_libvar -p DOC iorl -ml RL_LIB md efmath efx efm -lm

foreach -A files -d . -S h 'mf_file $tg $src'
foreach -s c 'mf_cc -I. -c $src'

mf_var OBJ "efeudoc.o skip.o parse.o p_reg.o p_block.o DocCopy.o docsym.o" \
	"doctab.o doc_cmd.o doc_mac.o" \
	"doc_tools.o doc_mark.o doc_env.o doc_copy.o doc_tab.o" \
	"doc_verb.o doc_input.o subin.o expand.o" \
	"tab_info.o tab_load.o tab.o tab_mac.o" \
	"DocDrv.o Doc.o docout.o test.o" \
	"html.o html_open.o html_putc.o html_cpush.o" \
	"html_tab.o html_cmd.o html_env.o" \
	"LaTeX.o latex.o latex_putc.o latex_cmd.o latex_env.o" \
	"mroff.o mroff_par.o mroff_var.o mroff_putc.o" \
	"mroff_tab.o mroff_cmd.o mroff_env.o" \
	"term.o term_var.o term_putc.o term_cmd.o term_env.o" \
	"listarg.o f_section.o setup.o"

mf_cc -o '$(BIN)/efeudoc' -mo OBJ -md DEP_DOC -ml LIB_DOC

foreach -m CFGDIR -S enc 'mf_file $tg $src'
foreach -m CFGDIR -S sym 'mf_file $tg $src'
foreach -m CFGDIR -S dmac 'mf_file $tg $src'
foreach -m CFGDIR -S cfg 'mf_file $tg $src'
foreach -m CFGDIR -S eps 'mf_file $tg $src'
foreach -m CFGDIR -S ltx 'mf_file $tg $src'

mf_var DOCBUF "DocBuf.o DocBuf_copy.o MacDef.o" \
	"s2d_parse.o s2d_skip.o s2d_decl.o copy.o" 

mf_var S2DOBJ "src2doc.o s2d_data.o s2d_eval.o s2d_copy.o" \
	"s2d.o s2d_std.o s2d_src.o s2d_sh.o \$(DOCBUF)"

mf_var EMOBJ "efeuman.o \$(DOCBUF)"
mf_cc -o '$(BIN)/src2doc' -mo S2DOBJ -md DEP_DOC -ml LIB_DOC
mf_cc -o '$(BIN)/efeuman' -mo EMOBJ -md DEP_DOC -ml LIB_DOC
foreach -m BIN -s esh 'mf_script -e -c esh $tg $src'

efeu_note -d latex

if [ "$HAS_LATEX" != 1 ]; then
	efeu_note latex <<!
LaTeX or dvips ist not installed and so PostScript-Versions of the
Documentations could not be build. Install the appropriate packages und
rerun the building of EFEU.
!
fi

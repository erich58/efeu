" Vim syntax file
" Language:	efeudoc
" Maintainer:	Erich Frühstück
" Last change:	1999/09/24

" Remove any old syntax stuff hanging around
syn clear
syn case match

"Sonderzeichen
"syn match docError	"&[_[:alnum:]]*"
syn match docSpecial	"&[_[:alpha:]][_[:alnum:]]*[ \t]*"
syn match docSpecial	"&[_[:alpha:]][_[:alnum:]]*;"
syn match docError	"&[^_[:alnum:]]"
syn match docSpecial	"&#[[:digit:]]*;"
syn match docSpecial	"&[-&;]"

syn match docTabKey	"\[.*\]"	contained
syn match docTabKey	"#[[:digit:]]*"	contained
syn match docTabKey	"#[[:digit:]]*[lrc]"	contained

"Listenmarken
syn match docEsc	"\\[\\\[\]]"
syn match docItem	"^[ \t]*[*][[:space:]]\+"
syn match docItem	"^[ \t]*[#][[:space:]]\+"
syn region docSubItem	matchgroup=Delimiter start="\[" end="\]" contained contains=docEsc,docSubItem
syn region docItem	matchgroup=Delimiter start="^[ \t]*\[" end="\]" contains=docEsc,docSubItem

"Textmakros
syn match docError	"@[^_[:alnum:]]"
"syn match docMac	"@[_[:alpha:]][_[:alnum:]]*"
syn match docMac	"@[_[:alpha:]][_[:alnum:]]*[ \t]*"
syn match docMac	"@[_[:alpha:]][_[:alnum:]]*;"

syn region docMacArg	matchgroup=docMac start="@[_[:alpha:]][_[:alnum:]]*[ \t]*(" end=")" contains=ALL

syn region docArg	matchgroup=Delimiter start="{" end="}" contained contains=ALL 

" Befehle
syn match docCommand	"\\[_[:alpha:]][_[:alnum:]]*[ \t]*"
syn match docCommand	"\\[_[:alpha:]][_[:alnum:]]*[ \t]*\[[^][]*\]"
syn match docCommand	"\\[_[:alpha:]][_[:alnum:]]*;"
syn match docCommand	"\\[^_[:alnum:]]"
syn match docContinue	"\\$"

"syn region docCmdArg	matchgroup=docCommand start="\\[_[:alpha:]][_[:alnum:]]*[ \t]*\[.*\]{" end="}" contains=ALL
"syn region docCmdArg	matchgroup=docCommand start="\\[_[:alpha:]][_[:alnum:]]*[ \t]*{" end="}" contains=ALL

syn region docDef	start="[\\@][_[:alpha:]][_[:alnum:]]*[ \t]*=" end="$"
syn region docDef	start="[\\@][_[:alpha:]][_[:alnum:]]*[ \t]*=[ \t]*$" end="^$"
syn region docDef	start="[\\@][_[:alpha:]][_[:alnum:]]*[ \t]*=[ \t]*{" end=".*" contains=docComment,docCommentError,docExpr
syn region docExpr	matchgroup=Delimiter start="{" end="}" contained contains=docComment,docCommentError,docExpr

" Parametersubstitution

"syn region docSubstArg	matchgroup=docSubst start="\$(" end=")" contains=ALL
"syn region docSubstArg	matchgroup=docSubst start="\$" end="$" contains=ALL
"syn region docSubstArg	matchgroup=docSubst start="\${" end="}" contains=ALL
syn match	docSubst	"\$[0-9*]"

syn match	docSpaceError	"\s*$"
syn match	docSpaceError	" \+\t"me=e-1

" Comments
syn region	docComment	start="/\*" end="\*/" contains=docSpaceError
syn region	docComment	start="//.*" end="$" contains=docContinue
syn match	docCommentError	"\*/"

" Befehlssequenzen
" syn region docCmdArg contained matchgroup=Statement start="<" skip="<<\|>>" end=">"

" syn match docCmdEsc	contained "<<"
" syn match docCmdEsc	contained ">>"

" syn region docCmd matchgroup=Delimiter start="|" skip="||" end="|" contains=docCmdArg,docCmdEsc
syn match docTabKey	"|"
syn region docCmd matchgroup=Delimiter start="|" skip="||" end="|"

" Textauszeichnungen
syn region docIT matchgroup=Delimiter start="<" end=">" contains=ALL
syn region docIT matchgroup=Delimiter start="</" end="/>" contains=ALL
syn region docRM matchgroup=Delimiter start="<=" end="=>" contains=ALL
syn region docRM matchgroup=Delimiter start="<'" end="'>" contains=ALL
syn region docRM matchgroup=Delimiter start=+<"+ end=+">+ contains=ALL
syn region docBF matchgroup=Delimiter start="<\*" end="\*>" contains=ALL
syn region docTT matchgroup=Delimiter start="<|" end="|>" contains=ALL

" Eingebundene Dokumente

syn region docBF matchgroup=Delimiter start="^-----*[ \t]*.*$" end="^----.*$"

"Tabellen

syn region docTab matchgroup=Delimiter start="\\tab.*$" end="^$" contains=docTabKey,docRM,docBF,docTT,docIT,docEsc,docSpecial,docError,docMac,docCommand,docContinue,docSubst

if !exists("did_doc_syntax_inits")
	let did_doc_syntax_inits = 1
	" The default methods for highlighting.  Can be overridden later
	hi docIT		term=italic ctermfg=Cyan gui=italic
	hi docBF		term=bold cterm=bold gui=bold
	hi docTT		term=underline ctermfg=Magenta guifg=#ffa0a0
	hi link docCommand	Statement
	hi link docArg		docBF
	hi link docCmd		docTT
	hi link docCmdEsc	docTT
	"hi link docCmdArg	docBF
	hi link docAttDelim	Special
	hi link docComment	Comment
	hi link docSpecial	Special
	hi link docEsc		Delimiter
	hi link docItem		Statement
	hi link docSubItem	Type
	hi link docMac		PreProc
	"hi link docMacArg	docBF
	hi link docDef		Type
	hi link docExpr		Type
	hi link docContinue	Special
	hi link docError	Error
	hi link docCommentError	Error
	hi link docSpaceError	Error
	hi link docSubst	Type
	hi link docSubstArg	docBF
	"hi link docTab		Type
	hi link docTabKey	Delimiter
	"hi link docSubstArg	Type
	set wrapmargin=10
	set autoindent
endif

syn match docCommand	"\\table*[ \t]*"
syn match docCommand	"\\table*[ \t]*\[.*\]"

let b:current_syntax = "doc"

" vim: ts=8 sts=8 sw=8 noet:

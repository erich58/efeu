" Vim syntax file
" Language:	efeudoc / Befehlsdefinitionen
" Maintainer:	Erich Frühstück
" Last change:	1999/09/24

" Remove any old syntax stuff hanging around
syn clear
syn case ignore

" Read the C syntax to start with
source <sfile>:p:h/doc.vim

syn match dmacProtect	"\\protect\\[[:alnum:]]*"

syn region dmacStr	matchgroup=Delimiter start="'" skip="\\'" end="'" contained
syn region dmacStr	matchgroup=Delimiter start=+"+ skip=+\\\\\|\\"+ end=+"+ contained
syn region dmacBlock	matchgroup=Delimiter start="{" end="}" contained contains=dmacStr,docComment,docCommentError,dmacBlock
syn region dmacExpr	matchgroup=dmacCmd start="{" end="}" contained contains=dmacStr,docComment,docCommentError,dmacBlock


syn region dmacCond	matchgroup=Delimiter start="\\if[ \t]" end="\\endif" contains=docCommand,docMakro,dmacExpr,dmacStr,dmacCmd,dmacCond,dmacDelim,dmacProtect
syn region dmacCond	matchgroup=Delimiter start="\\ifdef" end="\\endif" contains=docCommand,docMakro,dmacExpr,dmacStr,dmacCmd,dmacCond,dmacDelim,dmacProtect
syn region dmacCond	matchgroup=Delimiter start="\\ifndef" end="\\endif" contains=docCommand,docMakro,dmacExpr,dmacStr,dmacCmd,dmacCond,dmacDelim,dmacProtect
syn region dmacComment	matchgroup=Comment start="\\if[ \t][ \t]*0$" end="\\endif" contains=dmacComment
syn region dmacComment	matchgroup=Comment start="\\if" end="\\endif" contained

syn region dmacCmdline	matchgroup=Delimiter start="\\eval" end="$" contains=dmacExpr 
syn region dmacCmdline	matchgroup=Delimiter start="\\include" end="$"
syn region dmacCmdline	matchgroup=Delimiter start="\\def" end="$" contains=docCommand,docMakro,dmacExpr,dmacStr
syn region dmacCmdline	matchgroup=Delimiter start="\\def" end="$" contains=docCommand,docMakro,dmacExpr,dmacStr

syn match dmacCmd	"\\break"
syn match dmacCmd	"\\else" contained

if !exists("did_dmac_syntax_inits")
	let did_dmac_syntax_inits = 1
	hi link dmacCmd		Delimiter
	hi link dmacCmdline	Type
	hi link dmacProtect	docCommand
	hi link dmacComment	Comment
	hi link dmacCond	docBF
	hi link dmacStr		String
	hi link dmacExpr	docBF
	hi link dmacBlock	dmacExpr
	set wrapmargin=10
	set autoindent
endif

let b:current_syntax = "dmac"

" vim: ts=8 sts=8 sw=8 noet:

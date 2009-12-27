" Vim syntax file
" Language:	esh - Befehlsinterpreter
" Maintainer:	Erich Frühstück
" Last change:	2002/06/05

" Remove any old syntax stuff hanging around
syn clear
syn case ignore

" Read the C++ syntax to start with
if	version >= 506
	source $VIMRUNTIME/syntax/cpp.vim
else
	source $VIM/syntax/cpp.vim
endif

syn region eshMagic matchgroup=PreProc start="^#!" end="$"
"syn region eshRegExp matchgroup=String start="/" skip="\\/" end="/i*"
"syn region eshComment matchgroup=Comment start="/\*" end="*/"
"syn region eshComment matchgroup=Comment start="//" skip= "\\$" end="$"

syn region eshStr matchgroup=Delimiter start="string !$" end="^!" contains=cString,cInclude,cPreCondit,cComment

syn region eshStr matchgroup=Delimiter start="pconfig !$" end="^!" contains=cString,cInclude,cPreCondit,cComment

if !exists("did_esh_syntax_inits")
	let did_esh_syntax_inits = 1
	hi eshStr		term=bold cterm=bold gui=bold
	hi link	eshMagic	PreProc
	hi link	eshRegExp	String
	hi link	eshComment	Comment
	set wrapmargin=10
	set autoindent
endif

let b:current_syntax = "esh"

" vim: ts=8 sts=8 sw=8 noet:

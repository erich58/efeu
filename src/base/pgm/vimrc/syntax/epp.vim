" Vim syntax file
" Language:	C
" Maintainer:	Bram Moolenaar <Bram@vim.org>
" Last change:	1998 Aug 13

" Remove any old syntax stuff hanging around
syn clear

" A bunch of useful keywords
syn keyword	eppTodo		contained TODO FIXME XXX

" String constants
" Highlight special characters (those which have a backslash) differently
syn match	eppSpecial	contained "\\x\x\+\|\\\o\{1,3\}\|\\.\|\\$"
syn region	eppString		start=+"+ skip=+\\\\\|\\"+ end=+"+ contains=eppSpecial
syn region	eppString		start=+'+ skip=+\\\\\|\\'+ end=+'+ contains=eppSpecial

"catch errors caused by wrong parenthesis
syn cluster	eppParenGroup	contains=eppParenError,eppIncluded,eppSpecial,eppTodo
syn region	eppParen		transparent start='(' end=')' contains=ALLBUT,@eppParenGroup
syn match	eppParenError	")"
syn match	eppInParen	contained "[{}]"

" A comment can contain eppString and eppChar.
" But a "*/" inside a eppString in a eppComment DOES end the comment!  So we
" need to use a special type of eppString: eppCommentString, which also ends on
" "*/", and sees a "*" at the start of the line as comment again.
" Unfortunately this doesn't very well work for // type of comments :-(
syntax match	eppCommentSkip	contained "^\s*\*\($\|\s\+\)"
syntax region eppCommentString	contained start=+"+ skip=+\\\\\|\\"+ end=+"+ end=+\*/+me=s-1 contains=eppSpecial,eppCommentSkip
syntax region eppCommentString	contained start=+'+ skip=+\\\\\|\\'+ end=+'+ end=+\*/+me=s-1 contains=eppSpecial,eppCommentSkip
syntax region eppComment2String	contained start=+"+ skip=+\\\\\|\\"+ end=+"+ end="$" contains=eppSpecial
syntax region eppComment2String	contained start=+'+ skip=+\\\\\|\\'+ end=+'+ end="$" contains=eppSpecial
syntax region eppComment	start="/\*" end="\*/" contains=eppTodo,eppCommentString,eppChar
syntax match  eppComment	"//.*" contains=eppTodo,eppComment2String,eppChar

syn region	eppPreCondit	start="^\s*#\s*\(if\>\|ifdef\>\|ifndef\>\|elif\>\|else\>\|endif\>\)" skip="\\$" end="$" contains=eppComment,eppString,eppChar,eppCommentError
syn region	eppIncluded	contained start=+"+ skip=+\\\\\|\\"+ end=+"+
syn match	eppIncluded	contained "<[^>]*>"
syn match	eppInclude	"^\s*#\s*include\>\s*["<]" contains=eppIncluded
"syn match eppLineSkip	"\\$"
syn cluster	eppPreProcGroup	contains=eppPreCondit,eppIncluded,eppInclude,eppDefine,eppInParen
syn region	eppDefine		start="^\s*#\s*\(define\>\|undef\>\)" skip="\\$" end="$" contains=ALLBUT,@eppPreProcGroup
syn region	eppPreProc	start="^\s*#\s*\(pragma\>\|line\>\|warning\>\|warn\>\|error\>\)" skip="\\$" end="$" contains=ALLBUT,@eppPreProcGroup

"if !exists("epp_minlines")
"  let epp_minlines = 15
"endif
"exec "syn sync ccomment eppComment minlines=" . epp_minlines

if !exists("did_epp_syntax_inits")
  let did_epp_syntax_inits = 1
  " The default methods for highlighting.  Can be overridden later
"  hi link eppChar	Character
  hi link eppSpecialChar eppSpecial
  hi link eppParenError	eppError
  hi link eppInParen	eppError
  hi link eppCommentError	eppError
  hi link eppInclude	Include
  hi link eppPreProc	PreProc
  hi link eppDefine	Macro
  hi link eppIncluded	eppString
  hi link eppError	Error
  hi link eppPreCondit	PreCondit
  hi link eppCommentError	eppError
  hi link eppCommentString eppString
  hi link eppComment2String eppString
  hi link eppCommentSkip	eppComment
  hi link eppString	String
  hi link eppComment	Comment
  hi link eppSpecial	SpecialChar
  hi link eppTodo		Todo
endif

let b:current_syntax = "epp"

" vim: ts=8

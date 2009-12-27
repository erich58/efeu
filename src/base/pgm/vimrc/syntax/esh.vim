" Vim syntax file
" Language:	esh - Befehlsinterpreter
" Maintainer:	Erich Frühstück
" Last change:	2002/06/05

" Remove any old syntax stuff hanging around
syn clear

" A bunch of useful C keywords
syn keyword	cStatement	goto break return continue asm
syn keyword	cLabel		case default
syn keyword	cConditional	if else switch
syn keyword	cRepeat		while for do

syn keyword	cTodo		contained TODO FIXME XXX

" cCommentGroup allows adding matches for special things in comments
syn cluster	cCommentGroup	contains=cTodo

" String and Character constants
" Highlight special characters (those which have a backslash) differently
syn match	cSpecial	contained "\\\(x\x\+\|\o\{1,3}\|.\|$\)"
if !exists("c_no_utf")
  syn match	cSpecial	contained "\\\(u\x\{4}\|U\x\{8}\)"
endif
if exists("c_no_cformat")
  syn region	cString		start=+L\="+ skip=+\\\\\|\\"+ end=+"+ contains=cSpecial
  " cCppString: same as cString, but ends at end of line
  syn region	cCppString	start=+L\="+ skip=+\\\\\|\\"\|\\$+ excludenl end=+"+ end='$' contains=cSpecial
else
  syn match	cFormat		"%\(\d\+\$\)\=[-+' #0*]*\(\d*\|\*\|\*\d\+\$\)\(\.\(\d*\|\*\|\*\d\+\$\)\)\=\([hlL]\|ll\)\=\([diuoxXfeEgGcCsSpn]\|\[\^\=.[^]]*\]\)" contained
  syn match	cFormat		"%%" contained
  syn region	cString		start=+L\="+ skip=+\\\\\|\\"+ end=+"+ contains=cSpecial,cFormat
  " cCppString: same as cString, but ends at end of line
  syn region	cCppString	start=+L\="+ skip=+\\\\\|\\"\|\\$+ excludenl end=+"+ end='$' contains=cSpecial,cFormat
  hi link cFormat cSpecial
endif
hi link cCppString cString

syn match	cCharacter	"L\='[^\\]'"
syn match	cCharacter	"L'[^']*'" contains=cSpecial
syn match	cSpecialError	"L\='\\[^'\"?\\abfnrtv]'"
syn match	cSpecialCharacter "L\='\\['\"?\\abfnrtv]'"
syn match	cSpecialCharacter "L\='\\\o\{1,3}'"
syn match	cSpecialCharacter "'\\x\x\{1,2}'"
syn match	cSpecialCharacter "L'\\x\x\+'"

"when wanted, highlight trailing white space
if exists("c_space_errors")
  if !exists("c_no_trail_space_error")
    syn match	cSpaceError	excludenl "\s\+$"
  endif
  if !exists("c_no_tab_space_error")
    syn match	cSpaceError	" \+\t"me=e-1
  endif
endif

"catch errors caused by wrong parenthesis and brackets
syn cluster	cParenGroup	contains=cParenError,cIncluded,cSpecial,cCommentSkip,cCommentString,cComment2String,@cCommentGroup,cUserCont,cUserLabel,cBitField,cCommentSkip,cOctalZero,cCppOut,cCppOut2,cCppSkip,cFormat,cNumber,cFloat,cOctal,cOctalError,cNumbersCom

syn region	cParen		transparent start='(' end=')' contains=ALLBUT,@cParenGroup,cCppParen,cCppString
  " cCppParen: same as cParen but ends at end-of-line; used in cDefine
syn region	cCppParen	transparent start='(' skip='\\$' excludenl end=')' end='$' contained contains=ALLBUT,@cParenGroup,cParen,cString
syn match	cParenError	")"
"syn match	cErrInParen	contained "[{}]"

"integer number, or floating point number without a dot and with "f".
syn case ignore
syn match	cNumbers	transparent "\<\d\|\.\d" contains=cNumber,cFloat,cOctalError,cOctal
" Same, but without octal error (for comments)
syn match	cNumbersCom	contained transparent "\<\d\|\.\d" contains=cNumber,cFloat,cOctal
syn match	cNumber		contained "\d\+\(u\=l\{0,2}\|ll\=u\)\>"
"hex number
syn match	cNumber		contained "0x\x\+\(u\=l\{0,2}\|ll\=u\)\>"
" Flag the first zero of an octal number as something special
syn match	cOctal		contained "0\o\+\(u\=l\{0,2}\|ll\=u\)\>" contains=cOctalZero
syn match	cOctalZero	contained "\<0"
syn match	cFloat		contained "\d\+f"
"floating point number, with dot, optional exponent
syn match	cFloat		contained "\d\+\.\d*\(e[-+]\=\d\+\)\=[fl]\="
"floating point number, starting with a dot, optional exponent
syn match	cFloat		contained "\.\d\+\(e[-+]\=\d\+\)\=[fl]\=\>"
"floating point number, without dot, with exponent
syn match	cFloat		contained "\d\+e[-+]\=\d\+[fl]\=\>"
" flag an octal number with wrong digits
syn match	cOctalError	contained "0\o*[89]\d*"
syn case match

if exists("c_comment_strings")
  " A comment can contain cString, cCharacter and cNumber.
  " But a "*/" inside a cString in a cComment DOES end the comment!  So we
  " need to use a special type of cString: cCommentString, which also ends on
  " "*/", and sees a "*" at the start of the line as comment again.
  " Unfortunately this doesn't very well work for // type of comments :-(
  syntax match	cCommentSkip	contained "^\s*\*\($\|\s\+\)"
  syntax region cCommentString	contained start=+L\="+ skip=+\\\\\|\\"+ end=+"+ end=+\*/+me=s-1 contains=cSpecial,cCommentSkip
  syntax region cComment2String	contained start=+L\="+ skip=+\\\\\|\\"+ end=+"+ end="$" contains=cSpecial
  syntax region  cCommentL	start="//" skip="\\$" end="$" keepend contains=@cCommentGroup,cComment2String,cCharacter,cNumbersCom,cSpaceError
  syntax region cComment	start="/\*" end="\*/" contains=@cCommentGroup,cCommentString,cCharacter,cNumbersCom,cSpaceError
else
  syn region	cCommentL	start="//" skip="\\$" end="$" keepend contains=@cCommentGroup,cSpaceError
  syn region	cComment	start="/\*" end="\*/" contains=@cCommentGroup,cSpaceError
endif
" keep a // comment separately, it terminates a preproc. conditional
hi link cCommentL cComment
syntax match	cCommentError	"\*/"

"syn keyword	cOperator	sizeof
syn keyword	cType	void bool char str
syn keyword	cType	int int8_t int16_t int32_t int64_t
syn keyword	cType	unsigned uint8_t uint16_t uint32_t uint64_t
syn keyword	cType	float double
syn keyword	cType   va_list IO Type_t Expr_t Lval_t
syn keyword	cType   EfiVec List_t Func VirFunc ObjFunc
syn keyword	cType   Date Time

syn keyword	cStructure	struct enum typedef
syn keyword	cStorageClass	static extern const
syn keyword	cStorageClass	promotion restricted virtual inline

syn keyword	cConstant NULL true false iostd ioerr NaN Inf

syn region	cPreCondit	start="^\s*#\s*\(if\|ifdef\|ifndef\|elif\)\>" skip="\\$" end="$" end="//"me=s-1 contains=cComment,cCppString,cCharacter,cCppParen,cParenError,cNumbers,cCommentError,cSpaceError
syn match	cPreCondit	"^\s*#\s*\(else\|endif\)\>"
if !exists("c_no_if0")
  syn region	cCppOut		start="^\s*#\s*if\s\+0\>" end=".\|$" contains=cCppOut2
  syn region	cCppOut2	contained start="0" end="^\s*#\s*\(endif\>\|else\>\|elif\>\)" contains=cSpaceError,cCppSkip,@cCommentGroup
  syn region	cCppSkip	contained start="^\s*#\s*\(if\>\|ifdef\>\|ifndef\>\)" skip="\\$" end="^\s*#\s*endif\>" contains=cSpaceError,cCppSkip,@cCommentGroup
endif
syn region	cIncluded	contained start=+"+ skip=+\\\\\|\\"+ end=+"+
syn match	cIncluded	contained "<[^>]*>"
syn match	cInclude	"^\s*#\s*include\>\s*["<]" contains=cIncluded
"syn match cLineSkip	"\\$"
syn cluster	cPreProcGroup	contains=cPreCondit,cIncluded,cInclude,cDefine,cErrInParen,cErrInBracket,cUserLabel,cSpecial,cOctalZero,cCppOut,cCppOut2,cCppSkip,cFormat,cNumber,cFloat,cOctal,cOctalError,cNumbersCom,cString,cCommentSkip,cCommentString,cComment2String,@cCommentGroup,cParen,cBracket
syn region	cDefine		start="^\s*#\s*\(define\|undef\)\>" skip="\\$" end="$" contains=ALLBUT,@cPreProcGroup
syn region	cPreProc	start="^\s*#\s*\(pragma\>\|line\>\|warning\>\|warn\>\|error\>\)" skip="\\$" end="$" keepend contains=ALLBUT,@cPreProcGroup

" Highlight User Labels
syn cluster	cMultiGroup	contains=cIncluded,cSpecial,cCommentSkip,cCommentString,cComment2String,@cCommentGroup,cUserCont,cUserLabel,cBitField,cOctalZero,cCppOut,cCppOut2,cCppSkip,cFormat,cNumber,cFloat,cOctal,cOctalError,cNumbersCom,cCppParen,cCppBracket,cCppString
syn region	cMulti		transparent start='?' skip='::' end=':' contains=ALLBUT,@cMultiGroup
" Avoid matching foo::bar() in C++ by requiring that the next char is not ':'
syn cluster	cLabelGroup	contains=cUserLabel
syn match	cUserCont	"^\s*\I\i*\s*:$" contains=@cLabelGroup
syn match	cUserCont	";\s*\I\i*\s*:$" contains=@cLabelGroup
syn match	cUserCont	"^\s*\I\i*\s*:[^:]"me=e-1 contains=@cLabelGroup
syn match	cUserCont	";\s*\I\i*\s*:[^:]"me=e-1 contains=@cLabelGroup

syn match	cUserLabel	"\I\i*" contained

" Avoid recognizing most bitfields as labels
syn match	cBitField	"^\s*\I\i*\s*:\s*[1-9]"me=e-1
syn match	cBitField	";\s*\I\i*\s*:\s*[1-9]"me=e-1

if !exists("c_minlines")
  if !exists("c_no_if0")
    let c_minlines = 50	    " #if 0 constructs can be long
  else
    let c_minlines = 15	    " mostly for () constructs
  endif
endif
exec "syn sync ccomment cComment minlines=" . c_minlines

" esh Extensions

syn region cBracket transparent start="\[" end="]"
syn region cBracket matchgroup=Delimiter start="\[\s*{" end="}\s*]" contains=cBracket
syn region cBracket matchgroup=Delimiter start="\[\s" end="]" contains=cBracket
syn region cBracket matchgroup=Delimiter start="\[\s*$" end="]" contains=cBracket

syn region eshMagic matchgroup=PreProc start="^#!" end="$"
"syn region eshRegExp matchgroup=String start="/" skip="\\/" end="/i*"
"syn region eshComment matchgroup=Comment start="/\*" end="*/"
"syn region eshComment matchgroup=Comment start="//" skip= "\\$" end="$"

syn region eshStr matchgroup=Delimiter start="string !$" end="^!" contains=cString,cInclude,cPreCondit,cDefine,cComment

syn region eshStr matchgroup=Delimiter start="parsub !$" end="^!" contains=cString,cInclude,cPreCondit,cDefine,cComment

syn region eshStr matchgroup=Delimiter start="pconfig !$" end="^!" contains=cString,cInclude,cPreCondit,cDefine,cComment

if !exists("did_esh_syntax_inits")
	let did_esh_syntax_inits = 1
	hi link cLabel	Label
	hi link cUserLabel	Label
	hi link cConditional	Conditional
	hi link cRepeat	Repeat
	hi link cCharacter	Character
	hi link cSpecialCharacter cSpecial
	hi link cNumber	Number
	hi link cOctal	Number
	hi link cOctalZero	PreProc	" link this to Error if you want
	hi link cFloat	Float
	hi link cOctalError	cError
	hi link cParenError	cError
	hi link cErrInParen	cError
	hi link cErrInBracket	cError
	hi link cCommentError	cError
	hi link cSpaceError	cError
	hi link cSpecialError	cError
	hi link cOperator	Operator
	hi link cStructure	Structure
	hi link cStorageClass	StorageClass
	hi link cInclude	Include
	hi link cPreProc	PreProc
	hi link cDefine	Macro
	hi link cIncluded	cString
	hi link cError	Error
	hi link cStatement	Statement
	hi link cPreCondit	PreCondit
	hi link cType		Type
	hi link cConstant	Constant
	hi link cCommentError	cError
	hi link cCommentString cString
	hi link cComment2String cString
	hi link cCommentSkip	cComment
	hi link cString	String
	hi link cComment	Comment
	hi link cSpecial	SpecialChar
	hi link cTodo		Todo
	hi link cCppSkip	cCppOut
	hi link cCppOut2	cCppOut
	hi link cCppOut	Comment
	hi eshStr		term=bold cterm=bold gui=bold
	hi link	eshMagic	PreProc
	hi link	eshRegExp	String
	hi link	eshComment	Comment
	set wrapmargin=10
	set autoindent
endif

let b:current_syntax = "esh"

" vim: ts=8 sts=8 sw=8 noet:

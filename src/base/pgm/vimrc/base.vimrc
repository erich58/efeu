" basic vim extensions

augroup filetype
  au BufNewFile,BufRead	Imakefile,*imake	set ft=epp
  au BufNewFile,BufRead	*.pph		set ft=epp
  au BufNewFile,BufRead	*.src	set ft=cpp
  au BufNewFile,BufRead	*.sh	set ft=sh
  au BufNewFile,BufRead	*.make,*.smh	set ft=sh
  au BufNewFile,BufRead	*.app,*.esh,*.cfg,*.hdr	set ft=esh
  au BufNewFile,BufRead	*.tpl,*.def,*.ms	set ft=esh
  au BufNewFile,BufRead	*.etk			set ft=esh
  au BufNewFile,BufRead	*.dmac	set ft=dmac
  au BufNewFile,BufRead	*.doc,*.fig,*.[1-9]*,*hlp	set ft=doc
  au BufNewFile,BufRead	CHANGELOG*,README*	set ft=doc
augroup END

let myscriptsfile = "VIMDIR/scripts.vim"

augroup cprog
  " Remove all cprog autocommands
  au!

  " When starting to edit a file:
  "   For *.c and *.h files set formatting of comments and set C-indenting on.
  "   For other files switch it off.
  "   Don't change the order, it's important that the line with * comes first.
  autocmd BufRead *       set formatoptions=tcql nocindent comments&
  "autocmd BufRead *.c,*.h set formatoptions=croql cindent comments=sr:/*,mb:*,el:*/,://
  "autocmd BufRead *.c,*.h set formatoptions=croql cindent comments=s1:/*,mb:\0,e:*/,://
  autocmd BufRead *.c,*.h set formatoptions=croql cindent comments=f:/*,://
augroup END

set hlsearch

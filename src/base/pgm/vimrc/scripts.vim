" Vim support file to detect file types in scripts

if getline(1) =~ '#!/usr/bin/env esh'
	set ft=esh
elseif getline(3) =~ '^exec esh'
	set ft=esh
elseif getline(1) =~ 'exec shmkmf'
	set ft=sh
elseif getline(1) =~ '#!/usr/bin/env etk'
	set ft=esh
elseif getline(3) =~ '^exec etk'
	set ft=esh
elseif getline(1) =~ '#!/usr/bin/env wish'
	set ft=tcl
elseif getline(3) =~ '^exec wish'
	set ft=tcl
elseif getline(1) =~ '^/[*]'
	set ft=esh
endif

/*
:*: shared object
:de: Gemeinsam genutzte Objekte

$Copyright (C) 2006 Erich Frühstück
This file is part of EFEU.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this library; see the file COPYING.Library.
If not, write to the Free Software Foundation, Inc.,
59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
*/

#include <EFEU/procenv.h>
#include <EFEU/patcmp.h>
#include <EFEU/ftools.h>
#include <EFEU/io.h>
#include <EFEU/dl.h>
#include <EFEU/mkpath.h>

char *so_path = NULL;

#if	HAS_DLFCN

static int libname (const char *name)
{
	if	(strchr(name, '/') || patcmp("lib*.so*", name, NULL))
		return 0;

	return 1;
}

#endif

/*
:*:
The function |$1| is a wrapper around |dlopen|.
:de:
Die Funktion |$1| ist eine Hülle um |dlopen|.
*/

void *so_open (const char *name)
{
#if	HAS_DLFCN
	char *path;
	void *handle;
	
	if	(name == NULL)	return NULL;

	if	(!so_path)
		so_path = mkpath(ProgDir, NULL, "lib", NULL);

	path = fsearch(so_path, NULL, name, "so");

	if	(!path && libname(name))
	{
		char *p = msprintf("lib%s", name);
		path = fsearch(so_path, NULL, p, "so");
		memfree(p);
	}

	if	(path)
	{
		handle = dlopen(path, RTLD_GLOBAL|RTLD_NOW);
		memfree(path);
	}
	else if	(libname(name))
	{
		char *p = msprintf("lib%s.so", name);
		handle = dlopen(p, RTLD_GLOBAL|RTLD_NOW);
		memfree(p);
	}
	else	handle = dlopen(name, RTLD_GLOBAL|RTLD_NOW);

	if	(!handle)
		io_xprintf(ioerr, "dlopen: %s\n", dlerror());

	return handle;
#else
	io_xprintf(ioerr, "dlopen() not available\n");
	return NULL;
#endif
}

void so_close (void *handle)
{
#if	HAS_DLFCN
	if	(handle)
		dlclose(handle);
#else
	;
#endif
}

void loadlib (const char *lname, const char *fname)
{
#if	HAS_DLFCN
	void *handle;
	void (*init)(void);

	handle = so_open(lname);

	if	(handle == NULL || fname == NULL)
	{
		;
	}
	else if	((init = dlsym(handle, fname)) == NULL)
	{
		io_xprintf(ioerr, "dlsym: %s\n", dlerror());
		dlclose(handle);
	}
	else	init();
#else
	io_xprintf(ioerr, "loadlib: not implemented\n");
#endif
}

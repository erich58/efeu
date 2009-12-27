/*
Standardfunktionen

$Copyright (C) 1994 Erich Frühstück
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

#include <EFEU/ftools.h>
#include <EFEU/object.h>
#include <EFEU/cmdconfig.h>
#include <EFEU/printobj.h>
#include <EFEU/locale.h>
#include <EFEU/preproc.h>
#include <EFEU/pconfig.h>

#define	STR(n)	Val_str(arg[n])
#define	INT(n)	Val_int(arg[n])
#define	IO(n)	Val_io(arg[n])

/*	InfoNode Datentyp
*/

static EfiType Type_inode = REF_TYPE("InfoNode", InfoNode *);

static EfiObj *inode_prev (const EfiObj *base, void *data)
{
	InfoNode *inode = base ? Val_ptr(base->data) : NULL;
	InfoNode *prev = inode ? (inode->prev ? inode->prev : inode) : NULL;
	return NewPtrObj(&Type_inode, rd_refer(prev));
}

static EfiObj *inode_name (const EfiObj *base, void *data)
{
	InfoNode *inode = base ? Val_ptr(base->data) : NULL;
	return str2Obj(mstrcpy(inode->name));
}

static EfiObj *inode_label (const EfiObj *base, void *data)
{
	InfoNode *inode = base ? Val_ptr(base->data) : NULL;
	return str2Obj(inode && inode->label ?
		mpsubarg(inode->label, "ns", inode->name) : NULL);
}

static EfiObj *inode_desc (const EfiObj *base, void *data)
{
	InfoNode *info = base ? Val_ptr(base->data) : NULL;
	char *res;
	
	if	(!info)
	{
		res = NULL;
	}
	else if	(!info->func)
	{
		res = mpsubarg(info->par, "nss", info->name, info->label);
	}
	else
	{
		StrBuf *sb = sb_acquire();
		IO *io = io_strbuf(sb);
		info->func(io, info);
		io_close(io);
		res = sb_cpyrelease(sb);
	}
	
	return str2Obj(res);
}

static EfiObj *inode_dim (const EfiObj *base, void *data)
{
	InfoNode *info = base ? Val_ptr(base->data) : NULL;
	return int2Obj(info && info->list ? info->list->used : 0);
}

static EfiMember inode_var[] = {
	{ "prev", &Type_inode, inode_prev, NULL },
	{ "name", &Type_str, inode_name, NULL },
	{ "label", &Type_str, inode_label, NULL },
	{ "desc", &Type_str, inode_desc, NULL },
	{ "dim", &Type_int, inode_dim, NULL },
};

static void f_inode (EfiFunc *func, void *rval, void **arg)
{
	Val_ptr(rval) = GetInfo(NULL, Val_str(arg[0]));
}

static void f_index (EfiFunc *func, void *rval, void **arg)
{
	InfoNode *info = Val_ptr(arg[0]);
	int n = Val_int(arg[1]);

	if	(info && info->list && n < info->list->used)
		info = ((InfoNode **) info->list->data)[n];

	SetupInfo(info);
	Val_ptr(rval) = rd_refer(info);
}


/*
Abfragefunktionen
*/

static void f_info (EfiFunc *func, void *rval, void **arg)
{
	BrowseInfo(STR(0));
}

static void f_cinfo (EfiFunc *func, void *rval, void **arg)
{
	if	(InfoBrowser)
		InfoBrowser(STR(0));
	else	StdInfoBrowser(STR(0));
}

static void f_load (EfiFunc *func, void *rval, void **arg)
{
	LoadInfo(AddInfo(NULL, STR(0), NULL, NULL, NULL), STR(1));
}

static void f_getinfo (EfiFunc *func, void *rval, void **arg)
{
	StrBuf *sb = sb_acquire();
	IO *io = io_strbuf(sb);
	PrintInfo(io, GetInfo(NULL, STR(0)));
	io_close(io);
	Val_str(rval) = sb_cpyrelease(sb);
}

static void f_addinfo (EfiFunc *func, void *rval, void **arg)
{
	AddInfo(NULL, STR(0), STR(1), NULL, mstrcpy(STR(2)));
}


/*
Funktionstabelle
*/

static EfiFuncDef fdef_info[] = {
	{ 0, &Type_inode, "InfoNode (str name)", f_inode },
	{ FUNC_VIRTUAL, &Type_inode, "operator[] (InfoNode, int)", f_index },
	{ 0, &Type_void, "Info (str name = NULL)", f_info },
	{ 0, &Type_void, "ConsoleInfoBrowser (str name = NULL)", f_cinfo },
	{ 0, &Type_void, "AddInfo (str name, str label = NULL, str txt = NULL)",
		f_addinfo },
	{ 0, &Type_str, "GetInfo (str name)", f_getinfo },
	{ 0, &Type_void, "LoadInfo (str name, str file)", f_load },
};


/*
Initialisierung
*/

void CmdSetup_info(void)
{
	AddType(&Type_inode);
	AddEfiMember(Type_inode.vtab, inode_var, tabsize(inode_var));
	AddFuncDef(fdef_info, tabsize(fdef_info));
}

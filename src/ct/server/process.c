#include "CubeHandle.h"
#include <EFEU/parsearg.h>
#include <EFEU/printobj.h>
#include <ctype.h>

#define	FLG_NONE	0x0	/* Keine Prüfung auf Datenwürfel */
#define	FLG_BREAK	0x1	/* Abbruch der Verarbeitung */
#define	FLG_LOAD	0x2	/* Datenwürfel muss geladen sein */

#define	E_UDC	"Unknown data cube %#s.\n"	
#define	E_NOC	"No cube selected.\n"	
#define	E_CMD	"Unknown command %#s.\n"	

typedef struct {
	char *name;	/* command name */
	char *arg;	/* arguments */
	int flags;	/* Steuerflags */
	void (*eval) (CubeHandle *handle, char *arg);
	char *label;	/* command label */
	char *desc;	/* optional description */
} CubeCommand;

static void f_clist (CubeHandle *handle, char *arg);

static void f_list (CubeHandle *handle, char *arg)
{
	MapTab_list(handle);
}

static void f_use (CubeHandle *handle, char *arg)
{
	rd_deref(handle->md);
	handle->md = NULL;

	if	(!arg)	return;

	handle->md = MapTab_get(arg);

	if	(!handle->md)
		CubeHandle_error(handle, E_UDC, arg);
}

static void f_column (CubeHandle *handle, char *arg)
{
	md_setflag(handle->md, arg, 0, mdsf_mark, MDXFLAG_MARK, NULL, 0);
}

static void f_time (CubeHandle *handle, char *arg)
{
	md_setflag(handle->md, arg, 0, mdsf_mark, MDXFLAG_TIME, NULL, 0);
}

static void f_hide (CubeHandle *handle, char *arg)
{
	md_setflag(handle->md, arg, 0, mdsf_mark, MDXFLAG_HIDE, NULL, 0);
}

static void f_base (CubeHandle *handle, char *arg)
{
	md_allflag(handle->md, 0, NULL, 0, mdsf_clear, MDFLAG_BASE);
	md_setflag(handle->md, arg, 0, NULL, 0, mdsf_mark, MDFLAG_BASE);
}

static void f_select (CubeHandle *handle, char *arg)
{
	md_allflag(handle->md, 0, NULL, 0, mdsf_clear, MDFLAG_LOCK);
	md_setflag(handle->md, arg, 0, NULL, 0, mdsf_lock, MDFLAG_LOCK);
}

static void f_permut (CubeHandle *handle, char *arg)
{
	md_permut(handle->md, arg);
}

static void f_about (CubeHandle *handle, char *arg)
{
	CubeHandle_beg(handle, "TXT", NULL);
	utf8_put(GetResource("Ident", NULL), handle->out);
	io_putc('\n', handle->out);
	utf8_put(GetResource("Version", NULL), handle->out);
	io_putc('\n', handle->out);
	utf8_put(GetResource("Copyright", NULL), handle->out);
	io_putc('\n', handle->out);
}

/*
command table with descriptions
*/

static CubeCommand cdef[] = {
	{ "?", "[<name>]", FLG_NONE, f_clist, "list commands",
"Returns the synopsis of the given command name or a list of all\n"
"available commands.\n"
	},
	{ "q[uit]", NULL, FLG_BREAK, NULL, "quit connection",
"Quits the connection.\n"
	},
	{ "", "<empty line>", FLG_NONE, NULL, "continue data output",
"Continues the current data transfer.\n"
	},
	{ "n[ext]", "<lines>", FLG_NONE, NULL, "continue data output",
"Continues the current data transfer. The optional argument set the\n"
"time limit temporary for the next data block.\n"
	},
	{ "break", NULL, FLG_NONE, NULL, "break data output",
"Terminates the current data transfer.\n"
	},
	{ "set", "<name>|=|<val> ...", FLG_NONE, CubeHandle_set, "set parameters",
"Set given parameters to the specific value and lists the current settings.\n"
	},
	{ "list", NULL, FLG_NONE, f_list, "list data cubes",
"Gives a list of available data cubes.\n"
	},
	{ "use", "<name>", FLG_NONE, f_use, "set default data cube",
"Set the default data cube to name for query commands.\n"
	},
	{ "axis", "<list>", FLG_LOAD, CubeHandle_axis, "show axis",
"List the axis of the current data cube.\n"
	},
	{ "index", "<list>", FLG_LOAD, CubeHandle_index, "show index",
"List the labels of the axis defined by list. Without argument,\n"
"all axis are listed.\n"
	},
	{ "choice", "<axis>=<name>", FLG_LOAD, CubeHandle_choice,
		"choice description column",
"Choice description columns on axis which offers a list of different text.\n"
	},
	{ "permut[e]", "<list>", FLG_LOAD, f_permut, "reorder axes of data cube",
"Permutes the listed axes of the data cube in the given order. Axes not listed\n"
"are moved to the end with the original order unchanged.\n"
	},
	{ "col[umn]", "<list>", FLG_LOAD, f_column, "mark axes as column",
"Sets a two dimensional view of the data cube by marking some\n"
"axes as `column'. The opposite axes are `lines'. This has effects on\n"
"walking through the data. Columns varies faster than lines.\n"
"This is a simple form of virtual permutation.\n"
	},
	{ "hide", "<list>", FLG_LOAD, f_hide, "hide labels of given axis",
"Hides the labels of the corresponding axis on data output.\n"
"Without argument, all labels are visible.\n"
	},
	{ "sel[ect]", "<selection>", FLG_LOAD, f_select, "select part of data cube",
"Sets the visible area of a data cube. Without argument, the hole\n"
"data cube is selected.\n"
	},
	{ "print", "<selection>", FLG_LOAD, CubeHandle_print, "print data cube",
"Command to print the selected data with labels.\n"
"Selection is done both by a active selection from a previous select\n"
"command and the additional selection of the print command.\n"
	},
	{ "data", "<selection>", FLG_LOAD, CubeHandle_data, "choose data",
"Walks through the data cube and writes out the selected data with labels\n"
"and one value per line.\n"
"Selection is done both by a active selection from a previous select\n"
"command and the additional selection of the data command.\n"
	},
	{ "xdata", "<selection>", FLG_LOAD, CubeHandle_xdata,
		"choose data with base",
"Like data with the additional calculating of a corresponding base element\n"
"of the data cube. The base element is defined by the base command and is\n"
"independent of the other selections.\n"
	},
	{ "base", "<selection>", FLG_LOAD, f_base, "mark base elements",
"Selects the base for the xdata command\n"
	},
	{ "tdata", "<lag> <selection>", FLG_LOAD, CubeHandle_tdata,
		"choose data with time offset",
"This is another variant of the data command, where the corresponding\n"
"base element is calculated from the time axis (set with time) with a\n"
"given lag. Negative values give leads.\n"
	},
	{ "time", "<list>", FLG_LOAD, f_time, "set time axis",
"Defines the time axis for the tdata command.\n"
	},
	{ "about", NULL, FLG_NONE, f_about, "About mdmapd",
"Show actual version and copyright information.\n"
	},
};


static CubeCommand *get_command(char *name, char **arg)
{
	int i;
	char *p;

	for (i = 0; i < tabsize(cdef); i++)
	{
		if	((p = test_key(cdef[i].name, name)))
		{
			if	(arg)	*arg = p;

			return cdef + i;
		}
	}

	if	(arg)	*arg = NULL;

	return NULL;
}

static void f_synopsis (CubeCommand *cmd, IO *out)
{
	char *p;
	int stat;

	io_printf(out, "%s\n\n", cmd->label);
	io_puts("usage: ", out);

	if	(cmd->flags & FLG_LOAD)
		io_puts("[<cube>|.|]", out);
	
	for (stat = 0, p = cmd->name; *p; p++)
	{
		switch (*p)
		{
		case '[':
		case ']':
			if	(stat)
				io_putc('|', out);

			stat = 0;
			break;
		default:
			if	(!stat)
				io_putc('|', out);

			stat = 1;
			break;
		}

		io_putc(*p, out);
	}

	if	(stat)
		io_putc('|', out);

	if	(cmd->arg)
	{
		io_putc(' ', out);
		io_puts(cmd->arg, out);
	}

	io_puts("\n\n", out);

	if	(cmd->desc)
	{
		io_puts(cmd->desc, out);
		io_puts("\n", out);
	}
}

static void f_clist (CubeHandle *handle, char *arg)
{
	int i, k;

	if	(arg)
	{
		CubeCommand *cmd = get_command(arg, NULL);

		if	(!cmd)
		{
			CubeHandle_beg(handle, E_CMD, arg);
		}
		else
		{
			CubeHandle_beg(handle, "TXT", NULL);
			f_synopsis(cmd, handle->out);
		}

		return;
	}

	CubeHandle_beg(handle, "TXT", NULL);

	for (i = 0; i < tabsize(cdef); i++)
	{
		k = 0;

		if	(cdef[i].flags & FLG_LOAD)
			k += io_puts("[cube.]", handle->out);

		if	(cdef[i].name[0])
			k += io_puts(cdef[i].name, handle->out);
		else	k += io_puts(cdef[i].arg, handle->out);

		io_nputc(' ', handle->out, 16 - k);
		io_puts(cdef[i].label, handle->out);
		io_putc('\n', handle->out);
	}
}

char *trim_arg (char *arg)
{
	size_t n;

	if	(!arg)	return NULL;

	while (isspace((unsigned char) *arg))
		arg++;

	if	(*arg == 0)	return NULL;

	n = strlen(arg);

	while (n && isspace((unsigned char) arg[n-1]))
		arg[--n] = 0;

	return arg;
}

void process (int fd)
{
	CubeHandle *handle;
	CubeCommand *cmd;
	char *line, *arg;
	char *p;
	mdmat *md_save;
	int save;

	handle = CubeHandle_init(NULL, fd);

	while ((line = CubeHandle_read(handle)))
	{
		md_save = NULL;
		save = 0;

		for (p = line; *p; p++)
		{
			if	(*p == '.')
			{
				*p = 0;
				save = 1;
				md_save = handle->md;
				handle->md = MapTab_get(line);

				if	(!handle->md)
				{
					CubeHandle_error(handle, E_UDC, line);
					CubeHandle_send(handle);
					handle->md = md_save;
					continue;
				}

				line = p + 1;
			}
			else if	(isspace((unsigned char) *p))
			{
				break;
			}
		}

		cmd = get_command(line, &arg);

		if	(!cmd)
		{
			CubeHandle_error(handle, E_CMD, line);
		}
		else if	(*arg == '?')
		{
			CubeHandle_beg(handle, "TXT", NULL);
			f_synopsis(cmd, handle->out);
		}
		else if	((cmd->flags & FLG_LOAD) && !handle->md)
		{
			CubeHandle_error(handle, E_NOC);
		}
		else if	(cmd->flags & FLG_BREAK)
		{
			break;
		}
		else if	(cmd->eval)
		{
			CubeHandle_beg(handle, "RDY", "SEG");
			cmd->eval(handle, trim_arg(arg));
		}
		else	CubeHandle_beg(handle, "EOD", NULL);

		CubeHandle_send(handle);

		if	(save)
		{
			rd_deref(handle->md);
			handle->md = md_save;
		}
	}

	CubeHandle_close(handle);
}

void list_commands (IO *out)
{
	int i;

	for (i = 0; i < tabsize(cdef); i++)
	{
		if	(cdef[i].name[0])
		{
			char *p;

			io_puts("\\ttitem ", out);

			for (p = cdef[i].name; *p; p++)
				if (*p != '[' && *p != ']')
					io_putc(*p, out);

			io_putc('\n', out);
		}
		else	io_puts("\\item <empty line>\n", out);

		f_synopsis(cdef + i, out);
	}

	io_puts("\\end\n", out);
}

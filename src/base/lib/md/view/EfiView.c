/*
Sicht auf ein Efeu-Datenobjekt

$Copyright (C) 2007 Erich Frühstück
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

#include <EFEU/nkt.h>
#include <EFEU/Resource.h>
#include <EFEU/Info.h>
#include <EFEU/EfiView.h>
#include <EFEU/printobj.h>
#include <EFEU/TimeRange.h>
#include <EFEU/parsearg.h>
#include <ctype.h>

static Label Label_data = {
	"data",
	":*:base data"
	":de:Basisdaten"
};

static Label Label_view = {
	"view",
	":*:values of data view"
	":de:Variablen der Datensicht"
};


static Label Label_range = {
	"range",
	":*:time range index"
	":de:Zeitbereichsindex"
};

static Label Label_seq = {
	"seq",
	":*:sequence identificator"
	":de:Kennung für Ablauffolge"
};

static Label Label_date = {
	"t",
	":*:Event date"
	":de:Ereigniszeitpunkt"
};

static Label Label_val = {
	"val",
	":*:Event value"
	":de:Ereigniswert"
};

static Label Label_freq = {
	"freq",
	":*:Frequency data"
	":de:Häufigkeitsdaten"
};

static Label Label_var = {
	"var",
	":*:Variance data"
	":de:Varianzdaten"
};

static void EfiView_add (EfiView *view, Label *label, EfiType *type, void *ptr)
{
	VarTab_xadd(view->vtab,
		mstrcpy(label->name),
		mlangcpy(label->desc, NULL),
		type ? LvalObj(&Lval_ref, type, view, ptr) : ptr);
}

static char *view_ident (const void *ptr)
{
	const EfiView *par = ptr;
	return msprintf("%s", par->data ? par->data->type->name : NULL);
}

static void view_clean (void *ptr)
{
	EfiView *view = ptr;
	rd_deref(view->edb);
	UnrefObj(view->data);
	UnrefObj(view->base);
	UnrefObj(view->tg);
	UnrefObj(view->eval_obj);
	vb_free(&view->range_buf);
	rd_deref(view->vtab);
	rd_deref(view->func);
	rd_deref(view->out);
	memfree(view);
}

static RefType EfiView_reftype = REFTYPE_INIT("EfiView",
	view_ident, view_clean);


static void set_range (EfiView *view, const char *def);
static void set_eval (EfiView *view);

EfiView *EfiView_create (EfiObj *data, const char *method)
{
	EfiView *view;
	EfiViewPar *par;
	AssignArg *arg;
	EfiObj *base;
	char *range;
	double weight;
	char *p;

	if	(!data)	return NULL;

	base = NULL;

	if	(method && (isdigit((unsigned char) *method) || *method == '.'))
	{
		weight = C_strtod(method, &p);

		if	(*p == '*')
			p++;

		method = p;
	}
	else	weight = 1.;

	arg = assignarg(method, &range, " \t\n;");

	if	(arg)
	{
		if	((p = strchr(arg->name, ':')))
		{
			*p = 0;
			p++;

			if	(arg->name[0] == '?')
			{
				ShowVarTab(ioerr, "var", data->type->vtab);
				io_putc('\n', ioerr);
				exit(EXIT_SUCCESS);
			}

			PushVarTab(RefVarTab(data->type->vtab), RefObj(data));
			base = EvalObj(strterm(arg->name), NULL);
			PopVarTab();

			if	(!base || base->type == &Type_undef)
			{
				log_error(NULL,
					"Unbekannte Komponente \"$1\".\n",
					"s", arg->name);
				base = RefObj(data);
			}
		}
		else
		{
			base = RefObj(data);
			p = arg->name;
		}

		if	(*p == '?')
		{
			ListEfiPar(ioerr, base->type, &EfiPar_view, NULL, 0);
			exit(EXIT_SUCCESS);
		}

		if	(*p && strcmp(p, ".") != 0)
		{
			par = SearchEfiPar(base->type, &EfiPar_view, p);

			if	(!par)
			{
				log_error(NULL,
					"$1: Unbekannte Methode \"$2\".\n",
					"ss", base->type->name, p);
			}
		}
		else	par = NULL;
	}
	else	par = NULL;

	view = memalloc(sizeof *view);
	view->vtab = VarTab(NULL, 0);
	view->data = RefObj(data);
	view->base = RefObj(base ? base : data);
	view->edb = NULL;

	view->st = NULL;
	view->func = NULL;
	view->tg = NULL;

	view->eval_total = NULL;
	view->eval_range = NULL;
	view->eval_par = NULL;
	view->eval_obj = NULL;

	view->seq.id = 0;
	view->seq.rec = 0;
	view->seq.num = 0;

	view->date = 0;
	view->weight = weight;
	view->var.n = 0.;
	view->var.x = 0.;
	view->var.xx = 0.;

	view->nsave = 0;
	view->out = NULL;

	rd_init(&EfiView_reftype, view);
	EfiView_add(view, &Label_data, NULL, RefObj(view->data));
	set_range(view, range);
	EfiView_add(view, &Label_seq, &Type_EfiViewSeq, &view->seq);
	EfiView_add(view, &Label_date, &Type_Date, &view->date);
	EfiView_add(view, &Label_val, &Type_double, &view->var.x);
	EfiView_add(view, &Label_freq, &Type_FrequencyData, &view->var);
	EfiView_add(view, &Label_var, &Type_VarianceData, &view->var);

	if	(par && par->create)
		par->create(view, arg->opt, arg->arg);

	if	(view->eval_obj)
		EfiView_add(view, &Label_view, NULL, RefObj(view->eval_obj));

	set_eval(view);

	memfree(arg);
	return view;
}


void EfiView_process (EfiView *view)
{
	if	(view && view->eval_total)
	{
		view->seq.rec++;
		view->seq.num = 0;
		view->eval_total(view);
	}
}

void EfiView_event (EfiView *view, unsigned date, double val)
{
	view->seq.id++;
	view->seq.num++;
	view->date = date;
	view->var.n = view->weight;
	view->var.x = view->weight * val;
	view->var.xx = view->var.x * val;

	if	(view->func)
	{
		PushVarTab(RefVarTab(view->vtab), NULL);
		view->func->eval(view->func, view->tg->data,
			&view->data->data);
		PopVarTab();
	}

	if	(view->out)
	{
		WriteData(view->tg->type, view->tg->data, view->out);
		view->nsave++;
	}
	else
	{
		ShowObj(iostd, view->tg);
		io_putc('\n', iostd);
	}
}

void EfiView_var (EfiView *view, const char *def)
{
	if	(!def)	return;

	if	(*def == '?')
	{
		EfiView_list(ioerr, NULL, view->vtab);
		exit(EXIT_SUCCESS);
	}

	rd_deref(view->func);
	view->func = ConstructFunc(NULL, def, view->vtab);

	if	(view->func)
		view->tg = LvalObj(NULL, view->func->type);
	else	view->tg = RefObj(view->data);
}

/*
Set time range
*/

static void set_range (EfiView *view, const char *def)
{
	EfiType *type;
	size_t n;

	vb_init(&view->range_buf, 32, sizeof(TimeRange));
	strTimeRange(&view->range_buf, def);
	view->range_dim = view->range_buf.used;
	view->range = view->range_buf.data;
	view->range_idx = 0;

	if	(!view->range_dim)	return;

	type = NewEnumType(NULL, EnumTypeRecl(view->range_dim + 1));

	for (n = 0; n < view->range_dim; n++)
		AddEnumKey(type, mstrcpy(view->range[n].label), NULL, n);

	EfiView_add(view, &Label_range, type, &view->range_idx);
}


/*
set eval function
*/

static void eval_func (EfiView *view)
{
	for (view->range_idx = 0;
			view->range_idx < view->range_dim;
			view->range_idx++)
		view->eval_range(view, view->range + view->range_idx);
}

static void eval_range (EfiView *view)
{
	for (view->range_idx = 0;
			view->range_idx < view->range_dim;
			view->range_idx++)
		EfiView_event(view, view->range[view->range_idx].dat, 1.);
}

static void eval_record (EfiView *view)
{
	EfiView_event(view, 0, 1.);
}

static void set_eval (EfiView *view)
{
	if	(!view)	return;

	if	(view->eval_total)
	{
		;
	}
	else if	(view->eval_range)
	{
		if	(!view->range_dim)
			log_error(NULL, "Zeitbereich fehlt.\n", NULL);

		view->eval_total = eval_func;
	}
	else if	(view->range_dim)
	{
		view->eval_total = eval_range;
	}
	else	view->eval_total = eval_record;
}

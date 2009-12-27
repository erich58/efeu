/*	Scanner-Ausgabe korrigieren
	(c) 2000 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5
*/

#include <EFEU/pconfig.h>
#include <EFEU/mdmat.h>
#include <EFEU/data.h>
#include "korrscan.h"


/*	Resourcedefinitionen
*/

char *Mode = NULL;
char *Input = NULL;
char *Output = NULL;

Var_t vardef[] = {
	{ "Mode",	&Type_str, &Mode },
	{ "Input",	&Type_str, &Input },
	{ "Output",	&Type_str, &Output },
};


/*	Modustabelle
*/

typedef struct {
	char *name;
	char *desc;
	void (*eval) (Picture_t *pic, FILE *file);
} PicEval_t;

static void move_color (Picture_t *pic, FILE *file)
{
	AdjustPicture(pic, Adjust_red, 4);
	AdjustPicture(pic, Adjust_green, -4);
	SavePicture(pic, file);
}

static void move_3 (Picture_t *pic, FILE *file)
{
	AdjustPicture(pic, Adjust_red, 4);
	AdjustPicture(pic, Adjust_green, -3);
	SavePicture(pic, file);
}

static PicEval_t EvalTab[] = {
	{ "show", "Bilddatei anzeigen", ShowPicture },
	{ "copy", "Bilddatei kopieren", SavePicture },
	{ "korr", "Varben verschieben", move_color },
	{ "korr3", "Varben verschieben", move_3 },
};

static void ShowEval(FILE *file)
{
	int i;

	for (i = 0; i < tabsize(EvalTab); i++)
		fprintf(file, "%s\t%s\n", EvalTab[i].name, EvalTab[i].desc);
}

static PicEval_t *GetEval(const char *name)
{
	int i;

	for (i = 0; i < tabsize(EvalTab); i++)
		if	(mstrcmp(EvalTab[i].name, name) == 0)
			return EvalTab + i;

	return NULL;
}

int main (int narg, char **arg)
{
	Picture_t *pic;
	FILE *file;
	PicEval_t *eval;

	libinit(arg[0]);
	pconfig(NULL, vardef, tabsize(vardef));
	loadarg(&narg, arg);

	if	(Mode == NULL)
	{
		ShowEval(stderr);
		return 0;
	}

	eval = GetEval(Mode);

	if	(eval == NULL)
	{
		message(NULL, NULL, 2, 1, Mode);
		return EXIT_FAILURE;
	}

	file = fileopen(Input, "rz"); 
	pic = LoadPicture(file);
	fileclose(file);

	file = fileopen(Output, "wz"); 
	eval->eval(pic, file);
	fileclose(file);

	FreePicture(pic);
	return 0;
}

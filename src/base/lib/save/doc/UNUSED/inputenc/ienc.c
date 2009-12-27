/*	Eingabekonvertierung
	(c) 1999 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5
*/

#include <EFEU/InputEnc.h>
#include <EFEU/mstring.h>

#define	ENC_BSIZE	128

static void entry_clean (InputEncEntry_t *entry)
{
	memfree(entry->name);
	memfree(entry->value);
}

static InputEnc_t *enc_admin (InputEnc_t *tg, const InputEnc_t *src)
{
	if	(tg)
	{
		vb_clean(&tg->tab, (clean_t) entry_clean);
		vb_free(&tg->tab);
		memfree(tg->name);
		memfree(tg);
		return NULL;
	}
	else
	{
		tg = memalloc(sizeof(InputEnc_t));
		vb_init(&tg->tab, ENC_BSIZE, sizeof(InputEncEntry_t));
		return tg;
	}
}

static char *enc_ident (InputEnc_t *enc)
{
	return mstrcpy(enc->name);
}

REFTYPE(InputEnc_reftype, "InputEnc", enc_ident, enc_admin);

InputEnc_t *InputEnc (const char *name)
{
	InputEnc_t *enc = rd_create(&InputEnc_reftype);
	enc->name = mstrcpy(name);
	return enc;
}

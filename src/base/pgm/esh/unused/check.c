#include <EFEU/efmain.h>
#include <malloc.h>

static void mabort (enum mcheck_status status)
{
	const char *msg;

	switch (status)
	{
	case MCHECK_OK:
		msg = "memory is consistent, library is buggy";
		break;
	case MCHECK_HEAD:
		msg = "memory clobbered before allocated block";
		break;
	case MCHECK_TAIL:
		msg = "memory clobbered past end of allocated block";
		break;
	case MCHECK_FREE:
		msg = "block freed twice";
		break;
	default:
		msg = "bogus mcheck_status, library is buggy";
		break;
	}

	fprintf (stderr, "mcheck: %s\n", msg);
	fflush (stderr);
	/*
	abort ();
	*/
	libexit(1);
}

static void turn_on_mcheck (void)
{
	mcheck (mabort);
}

void (*__malloc_initialize_hook) (void) = turn_on_mcheck;

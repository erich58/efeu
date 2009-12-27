/*	Neuen Informationsknoten generieren
	(c) 1998 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

#include <EFEU/Info.h>
#include <EFEU/mstring.h>

#define	NODE_BSIZE	64

static InfoNode_t *node_free = NULL;
static int node_used = 0;
static int node_alloc = 0;

InfoNode_t *NewInfo (InfoNode_t *root, char *name)
{
	InfoNode_t *node;

	if	(node_free == NULL)
	{
		int i;

		node = node_free = lmalloc(NODE_BSIZE * sizeof(InfoNode_t));

		for (i = 1; i < NODE_BSIZE; i++)
		{
			node->prev = node + 1;
			node++;
		}

		node->prev = NULL;
		node_alloc += NODE_BSIZE;
	}

	node = node_free;
	node_free = node->prev;
	node_used++;

	memset(node, 0, sizeof(InfoNode_t));
	node->prev = root;
	node->name = name;
	return node;
}

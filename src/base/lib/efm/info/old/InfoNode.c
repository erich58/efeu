/*	Informationsknoten
	(c) 1998 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

#include <EFEU/Info.h>
#include <EFEU/mstring.h>


static int cmp_node (const InfoNode_t **a, const InfoNode_t **b)
{
	return mstrcmp((*a)->name, (*b)->name);
}

static InfoNode_t *info_root = NULL;


InfoNode_t *InfoNodeRoot (InfoNode_t *node)
{
	if	(node)		return node;
	if	(info_root)	return info_root;

	info_root = NewInfoNode(NULL, NULL);
	return info_root;
}


InfoNode_t *GetInfoNode (InfoNode_t *prev, char *name)
{
	InfoNode_t *node, **ptr;

	prev = InfoNodeRoot(prev);

	if	(name == NULL)	return prev;

	node = NewInfoNode(prev, name);
	ptr = vb_search(&prev->list, &node, (comp_t) cmp_node, VB_ENTER);
	DelInfoNode(&node);
	return *ptr;
}


InfoNode_t *FindInfoNode (InfoNode_t *prev, const char *name)
{
	InfoNode_t buf, *node, **ptr;

	prev = InfoNodeRoot(prev);

	if	(name == NULL)	return prev;

	buf.name = (char *) name;
	node = &buf;
	ptr = vb_search(&prev->list, &node, (comp_t) cmp_node, VB_SEARCH);
	return ptr ? *ptr : NULL;
}

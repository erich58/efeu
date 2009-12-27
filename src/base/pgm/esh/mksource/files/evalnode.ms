/*	Auswertungskonoten
	(c) 2000 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5
*/

#ifndef	_MS_EVALNODE
#define	_MS_EVALNODE	1

#include <syntools.ms>

struct EvalNode {
	str name;	/* Klassenname */
	str type;	/* Parametertype */
};

EvalNode EvalNode (str name, str type)
	return { psub(name), psub(type) };

inline void EvalNode::eval(str _name, str _type, str _body)
	LocalFunction("void $1_eval", "$2_t *node, $3_t *data",
		_body, psub(_name), psub(_type), type);

inline void EvalNode::info(str _name, str _type, str _body)
	LocalFunction("void $1_info", "$2_t *node, io_t *io",
		_body, psub(_name), psub(_type));

inline void EvalNode::clean(str _name, str _type, str _body)
	LocalFunction("void $1_clean", "$2_t *node",
		_body, psub(_name), psub(_type));

void EvalNode::class(str _name, str _body)
	LocalFunction("void *$1_class", "char *name, $2_t *data, size_t dim",
		_body, psub(_name), type);

void EvalNode::write (str body, bool glob = true)
{
	Function f = Function("void *$1", "$2_t *data, size_t dim", name, type);
	f.body(body);
	f.write(glob);
}

#endif	/* MS_EVALNODE */


/*	Verarbeitungsstack
*/

typedef struct ExecStack_s ExecStack_t;

struct ExecStack_s {
	ExecStack_t *prev;	/* Übergeordneter Stack */
	CmdTab_t *tab;		/* Aktuelle Befehlstabelle */
};

extern ExecStack_t ExecStack;

void PushExecStack(void);
void PopExecStack(void);


/*	Variablentabelle
	(c) 1996 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

#ifndef	_MS_ESHVARDEF
#define	_MS_ESHVARDEF	1

struct EshVarDefLine {
	str type;	/* Datentype */
	str name;	/* Name */
	str value;	/* Zuweisungswert */
};

void EshVarDefLine::decl(bool global = false)
{
	str type;

	if	(this.type == "str")	type = "char *";
	else if	(this.type == "bool")	type = "int ";
	else				type = this.type + " ";

	if	(global)
		fprintf(hdr, "%s%s;\n", type, this.name);
	else	src << "static ";

	src << type;
	src << this.name;

	if	(this.value != NULL)
		src << " = " << this.value;

	src << ";\n";
}

struct EshVarDef {
	str name;
	DataBase db;
};

EshVarDef EshVarDef(str name)	return { name, DataBase(EshVarDefLine) };

EshVarDef EshVarDef::load (IO def)
{
	str type, name, value;

	while (getline(def, type, name, value))
		this.db += { type, name, psub(value) };

	return this;
}

EshVarDef EshVarDef::write (bool global = false)
{
	src << '\n';

	for (x in this.db)
		x.decl(global);

	fprintf(src, "\nVar_t %s[] = {\n", this.name);

	for (x in this.db)
		fprintf(src, "\t{ %#s, &Type_%s, &%s },\n",
			x.name, x.type, x.name);

	src << "};\n\n";
	return this;
}

void EshVarTab(str name, str def, bool glob = false)
{
	EshVarDef v = EshVarDef(name);
	v.load(def);
	v.write(glob);
}

#endif	/* _MS_ESHVARDEF */

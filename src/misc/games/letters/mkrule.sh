#!/bin/sh
# Regeln fuer Symlinks generieren

if [ $# -ne 2 ]; then
	echo "usage: $0 image-dir cfg"
	exit 1
fi

printf 'DIR = %s\n' "$1"

LinkImage()
{
	printf '\nall::\n'
	printf '\t@mkdir -p $(DIR)/%s\n' "$1"
	printf '\tcd $(DIR)/%s && ln -fs ../../images/%s %s\n' "$1" "$3" "$2"
	printf '\nclean::\n'
	printf '\trm -f $(DIR)/%s/%s\n' "$1" "$2"
}

. ./$2

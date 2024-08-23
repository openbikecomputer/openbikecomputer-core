#!/bin/sh

running_folder=$(dirname ${0})
src_folder="${running_folder}/../../src"
textdomain="openbikecomputer"
locales_list="en_GB fr_FR"
src_files_list=$(find ${src_folder} -iname "*.[c|h]")

build() {
	for locale in ${locales_list}
	do
		echo "Build: ${locale}"
		output_folder="${running_folder}/${locale}/LC_MESSAGES"
		mo_file="${output_folder}/${textdomain}.mo"
		po_file="${running_folder}/${locale}/${textdomain}.po"

		mkdir -p ${output_folder}
		msgfmt -o ${mo_file} ${po_file}
	done
}

update() {
	for locale in ${locales_list}
	do
		echo "Update: ${locale}"
		pot_file="${running_folder}/${locale}/${textdomain}.pot"
		po_file="${running_folder}/${locale}/${textdomain}.po"

		xgettext  --force-po --default-domain=${textdomain} --keyword=_ --language=C --add-comments -o ${pot_file} ${src_files_list}
		msgmerge --lang ${locale} --sort-output --previous --no-fuzzy-matching --no-wrap --force-po --update ${po_file} ${pot_file}
	done
}

usage() {
	echo "usage: $0 <option>"
	echo "option: build, update"
}

# Check if we did give an argument
if [ "$#" -ne 1 ]
then
	usage
	exit 1
fi

# Check if we need to update or build translations
case $1 in
   "build")
      build
      ;;
   "update")
      update
      ;;
   *)
     usage
     ;;
esac

exit 0

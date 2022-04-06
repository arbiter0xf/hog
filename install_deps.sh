#!/bin/bash

set -e

readonly project_root="$(dirname $(readlink -f $0))"
readonly limine_dirname="limine"
readonly deps_dirname="hog_dependencies"
readonly deps_absolute="${project_root}/../${deps_dirname}"
readonly limine_absolute="${deps_absolute}/${limine_dirname}"
readonly limine_url="https://github.com/limine-bootloader/limine.git"

function setup_limine() {
	echo "Setting up Limine"
	echo "Installing Limine prerequisites"

	sudo apt install \
		make \
		grep \
		sed \
		findutils \
		gawk \
		gzip \
		mtools \
		autoconf \
		automake \
		texinfo \
		xorriso \
		nasm

	git clone ${limine_url} ${limine_absolute}

	pushd ${limine_absolute}
	git checkout -b v3.0-rc5 refs/tags/v3.0-rc5
	./make_toolchain.sh
	./autogen.sh
	make
	popd
}

function main() {
	if [ ! -d ${deps_absolute} ] ; then
		mkdir ${deps_absolute}
	fi

	if [ ! -d ${limine_absolute} ] ; then
		setup_limine
	fi
}

main

#!/usr/bin/env bash

progname=$(basename $0)

usage() {
	echo "usage: ${progname}"
	exit 1
}

errs=

while getopts ":" opt
do
	case "${opt}" in
	\?)
		echo "${progname}: ${OPTARG}: invalid option" >&2
		errs="yes"
		;;
	esac
done

[ -n "${errs}" ] && usage

shift $(($OPTIND - 1))
[ $# -ne 0 ] && usage

out="/tmp/$$out"
err="/tmp/$$err"

cleanup() {
	rm -f "${out}" "${err}"
	exit "${1}"
}

trap "cleanup 2" 1 2 3 15

awk -f test0.awk >"${out}" 2>"${err}"
errs="${?}"

[ "${errs}" != 0 ] && cleanup 2

cmp -s "${out}" test0.out || cleanup 2
cmp -s "${err}" test0.err || cleanup 2

cleanup 0

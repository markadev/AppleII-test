#!/usr/bin/env bash

set -e

AC="${AC:-java -jar AppleCommander-ac-1.8.0.jar}" 

if [ $# -lt 3 ]; then
	echo "Usage: $0 <input>.dsk <dest>.dsk [filenames...]"
	exit 1
fi

INPUT_DISK="$1"
shift
OUTPUT_DISK="$1"
shift

WORKING_DISK="${OUTPUT_DISK%.*}.tmp.${OUTPUT_DISK##*.}"
cp -f "${INPUT_DISK}" "${WORKING_DISK}"

for IN in "$@"; do
	case ${IN} in
	*#??????)
		NAME=$(basename "${IN%%#*}")
		META="${IN##*#}"
		ADDR="0x${META:2}"
		case "${META:0:2}" in
		"04")
			TYPE="txt"
			;;
		"fc"|"FC")
			TYPE="bas"
			;;
		"ff"|"FF")
			TYPE="sys"
			;;
		*)
			TYPE="bin"
		esac
		echo "${NAME}  ${TYPE}@${ADDR}"
		${AC} -p "${WORKING_DISK}" "${NAME}" "${TYPE}" "${ADDR}" < "${IN}"
		;;
	*.bas)
		NAME=$(basename "${IN%.bas}")
		echo "${NAME}  bas"
		${AC} -bas "${WORKING_DISK}" "${NAME}" < "${IN}"
		;;
	*)
		echo "Unknown file type"
		exit 1
	esac
done

mv -f "${WORKING_DISK}" "${OUTPUT_DISK}"

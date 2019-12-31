#Originally from https://balau82.wordpress.com/2010/10/06/trace-and-profile-function-calls-with-gcc/
#With additional changes by Daniel Fokkinga
#!/bin/bash
if test ! -f "$1"
then
	echo "Error: executable $1 does not exist."
	exit 1
fi
if test ! -f "$2"
then
	echo "Error: trace log $2 does not exist."
	exit 1
fi
EXECUTABLE="$1"
TRACELOG="$2"
while read LINETYPE FADDR CADDR CTIME; do
	FNAME="$(addr2line -f -e ${EXECUTABLE} ${FADDR}|head -1)"
	CDATE="$(date -Iseconds -d @${CTIME})"
	if test "${LINETYPE}" = "e"
	then
	CNAME="$(addr2line -f -e ${EXECUTABLE} ${CADDR}|head -1)"
	CLINE="$(addr2line -s -e ${EXECUTABLE} ${CADDR})"
	if [ "${FNAME}" != "main" ]
	then
		printf "Call(${FNAME}) "
 fi
 fi
	if test "${LINETYPE}" = "x"
	then
		if [ "${FNAME}" != "main" ]
		then
			printf "Ret(${FNAME}) "
 fi
 fi
done < "${TRACELOG}"
echo ""

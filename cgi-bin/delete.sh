#!/usr/bin/env bash

if [ -z "$QUERY_STRING"  && -z "$1"]
then
	message="No file specified"
	status="400"
else
	if [ -z $1 ]
	then
		FILENAME=$(echo "$QUERY_STRING" | sed -n 's/.*=\([^&]*\).*/\1/p' | sed 's/%20/ /g')
	else
		FILENAME="$1"
	fi
fi

if [ ! -z $status ]
then
	rm $FILENAME 2> /dev/null > /dev/null 

	status=`echo $?`

	if [ $status -eq 0 ]
	then
		message="File deleted successfully"
		status="200"
	else
		message="File could not be deleted"
		status="500"
	fi
fi

date=`date -u +"%a, %d %b %Y %H:%M:%S GMT"`

body="<html>
	<body>
		<h1>$message</h1>
	</body>
</html>\r\n"

content_length=${#body}

header="HTTP/1.1 $status\r\n\
Content-Length: $content_length\r\n\
Content-type: text/html; charset=utf-8\r\n\
Date: $date\r\n\
Last-Modified: $date\r\n\
Connection: close\r\n\
Server: CODAM_WEBSERV\r\n\r"
# Server: "$HOST"\r\n\r"

echo -e "$header"
echo -e "$body"
echo -e "\0"

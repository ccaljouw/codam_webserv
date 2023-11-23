#!/usr/bin/env bash

status="200"
message="File deleted successfully"
filenmame=""

# checks if the query string is empty and if no argument was given
if [[ -z "$QUERY_STRING"  && -z "$1" ]]
then
	message="No file specified"
	status="400"
else
	# checks if an argument was given
	if [ -z $1 ]
	then
		# gets the filename from the query string
		filename=`echo "$QUERY_STRING" | awk -F'=' '{print $2}' | cut -d'&' -f1 | sed 's/%20/ /g'`
	else
		# gets the filename from the argument depending on the format
		if [ -z `echo "$1" | grep "="` ]
		then
			filename="$1"
		else
			filename=`echo "$1" | awk -F'=' '{print $2}' | sed 's/%20/ /g'`
		fi
	fi
fi

# checks if status was previously set
if [ ! -z $status ]
then
	# checks if the file exists
	if [ -f "./uploads/$filename" ]
	then
		rm "./uploads/$filename" 2> /dev/null > /dev/null 

		status=`echo $?`

		# checks if the file was deleted successfully
		if [ ! $status -eq 0 ]
		then
			message="File could not be deleted"
			status="500"
		fi
	else
		message="File does not exist"
		status="404"
	fi
fi

# sets the date
date=`date -u +"%a, %d %b %Y %H:%M:%S GMT"`

body="<html>
	<head>
		<link rel="icon" href="data:,">
		<title>File Upload</title>
	</head>
	<body>
		<h1>$message</h1>
	</body>
</html>\r\n"

# gets the length of the body
content_length=${#body}

header="HTTP/1.1 $status\r
Content-Length: $content_length\r
Content-type: text/html; charset=utf-8\r\
Date: $date\r
Last-Modified: $date\r
Connection: close\r
Server: CODAM_WEBSERV\r\n\r"
# Server: $HOST\r\n\r"

echo -e "$header"
echo -e "$body"
echo -e "\0"

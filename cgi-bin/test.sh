#!/usr/bin/env sh

date=`date -u +"%a, %d %b %Y %H:%M:%S GMT"`
date=`date -u +"%a, %d %b %Y %H:%M:%S GMT"`

body="<!DOCTYPE html>
<html>
	<head>
		<link rel=\"icon\" href=\"data:,\">
		<title>Bash Test</title>
	</head>
	<body>
		<h1>This script was written in bash</h1>
	</body>
</html>\r\n\r"

content_length=`echo "$body" | wc -c`

header="HTTP/1.1 200\r
Content-Length: $content_length\r
Content-type: text/html; charset=utf-8\r
Date: $date\r
Last-Modified: $date\r
Connection: close\r
Server: $SERVER\r\n\r"

echo "$header"
echo "$body"
echo "\0"

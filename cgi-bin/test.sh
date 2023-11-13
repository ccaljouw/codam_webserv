#!/usr/bin/env bash

date=$(date -u +"%a, %d %b %Y %H:%M:%S GMT")

body="<html><body><h1>It works!</h1></body></html>\r\n"

content_length=${#body}

header="HTTP/1.1 200\r\n\
Content-Length: $content_length\r\n\
Content-type: text/html; charset=utf-8\r\n\
Date: $date\r\n\
Last-Modified: $date\r\n\
Server: CODAM_WEBSERV\r\n\r"

echo -e "$header"
echo -e "$body"
echo -e "\0"
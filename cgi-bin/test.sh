#!/usr/bin/env bash

date=`date -u +"%a, %d %b %Y %H:%M:%S GMT"`

body="<html>
	<head>
		<link rel=\"icon\" href=\"data:,\">
		<title>Bash Test</title>
	</head>
	<body>
		<h1>This script was written in bash</h1>
	</body>
</html>\r\n\r"

# content_length=${#body}

content_length=`echo -e "$body" | wc -c | tr -d ' '`

header="HTTP/1.1 200\r
Content-Length: $content_length\r
Content-type: text/html; charset=utf-8\r
Date: $date\r
Last-Modified: $date\r
Connection: close\r
Server: CODAM_WEBSERV\r\n\r"

echo -e "$header"
echo -e "$body"
echo -e "\0"



# #!/usr/bin/env bash

# date=$(date -u +"%a, %d %b %Y %H:%M:%S GMT")

# body="<html><body><h1>It works!</h1></body></html>\r\n"

# content_length=${#body}

# header="HTTP/1.1 200\r\n\
# Content-Length: $content_length\r\n\
# Content-type: text/html; charset=utf-8\r\n\
# Date: $date\r\n\
# Last-Modified: $date\r\n\
# Server: CODAM_WEBSERV\r\n\r"

# echo -e "$header"
# echo -e "$body"
# echo -e "\0"
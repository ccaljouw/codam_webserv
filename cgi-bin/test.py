#!/usr/bin/env python3

import datetime

x = datetime.datetime.now()
date = x.strftime("%a, %d %b %Y %H:%M:%S GMT")
body = """<!DOCTYPE html>
<html>
	<head>
		<link rel="icon" href="data:,">
		<title>Python Test</title>
	</head>
	<body>
		<h1>This script was written in python</h1>
	</body>
</html>\r\n"""

header = f"""HTTP/1.1 200\r
Content-Length: {len(body)}\r
Content-type: text/html; charset=utf-8\r
Date: {date}\r
Last-Modified: {date}\r
Connection: close\r
Server: CODAM_WEBSERV\r\n\r"""

print(header)
print(body)

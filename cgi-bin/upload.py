#!/usr/bin/env python3

import datetime, os, cgi, cgitb

cgitb.enable()

form = cgi.FieldStorage()

if 'filename' in form:
	fileitem = form['filename']
	fn = os.path.basename(fileitem.filename.replace("\\", "/"))
	uploadDir = os.getcwd() + '/uploads/'
	with open(os.path.join(uploadDir, fn), 'wb') as f:
		f.write(fileitem.file.read())
	message = fn + '" uploaded successfully'
else:
	message = 'Upload failed'

x = datetime.datetime.now()
date = x.strftime("%a, %d %b %Y %H:%M:%S GMT")

body = f"""<html>
	<body>
		<h1>{message}</h1>
	</body>
</html>\0"""

header = f"""HTTP/1.1 200\r
Content-Length: {len(body)}\r
Content-type: text/html; charset=utf-8\r
Date: {date}\r
Last-Modified: {date}\r
Server: CODAM_WEBSERV\r\n\r"""

print(header)
print(body)

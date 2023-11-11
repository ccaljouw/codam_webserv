#!/usr/bin/env python3

import datetime, os, cgi, cgitb, sys

form = cgi.FieldStorage()

for value in form.keys():
	print(f"idk {value}", file=sys.stderr)

if 'filename' in form:
	print("We have a filename", file=sys.stderr)
	fileitem = form['filename']
	fn = os.path.basename(fileitem.filename.replace("\\", "/"))
	uploadDir = os.getcwd() + '/uploads/'
	with open(os.path.join(uploadDir, fn), 'wb') as f:
		f.write(fileitem.file.read())
	message = fn + '" uploaded successfully'
else:
	print("Sad no filename", file=sys.stderr)
	message = 'Upload failed'

x = datetime.datetime.now()
date = x.strftime("%a, %d %b %Y %H:%M:%S GMT")

body = f"""<html>
	<body>
		<h1>{message}</h1>
	</body>
</html>"""

header = f"""HTTP/1.1 200\r
Content-Length: {len(body.encode("utf-8"))}\r
Content-type: text/html; charset=utf-8\r
Date: {date}\r
Last-Modified: {date}\r
Server: CODAM_WEBSERV\r\n\r"""

print(header)
print(body)
print("\0")

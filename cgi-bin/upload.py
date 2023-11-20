#!/usr/bin/env python3

import datetime, os, cgi, cgitb, sys

cgitb.enable(display=1, logdir="./logs", format="text")
form = cgi.FieldStorage()

# print(os.environ.get('REQUEST_METHOD'), file=sys.stderr)
print(os.environ.get('CONTENT_TYPE'), file=sys.stderr)

# cgi.print_form(form)
for key in form.keys():
	print(key, file=sys.stderr)

if form.getvalue('filename'):
	print("We have a filename", file=sys.stderr)
	fileitem = form['filename']
	fn = os.path.basename(fileitem.filename.replace("\\", "/"))
	uploadDir = os.getcwd() + '/uploads'
	with open(os.path.join(uploadDir, fn), 'wb') as f:
		f.write(fileitem.file.read())
	message = fn + ' uploaded successfully'
	status = 200
else:
	print("Sad no filename", file=sys.stderr)
	message = 'Upload failed!!!!'
	status = 500

x = datetime.datetime.now()
date = x.strftime("%a, %d %b %Y %H:%M:%S GMT")

body = f"""<html>
	<body>
		<h1>{message}</h1>
		<link rel="icon" href="data:,">
	</body>
</html>"""

header = f"""HTTP/1.1 {status}\r
Content-Length: {len(body)}\r
Content-type: text/html; charset=utf-8\r
Date: {date}\r
Last-Modified: {date}\r
Server: CODAM_WEBSERV\r\n\r"""

print(header)
print(body)
print("\0")

# cgi.print_environ()
# cgi.print_form(form)
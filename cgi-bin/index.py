#!/usr/bin/python3

import pathlib, datetime, os, sys

dirs = ["data", "html", "cgi-bin", "uploads"]

files = []
def listFiles(path, depth):
	for f in path.iterdir():
		if f.is_file() and depth != 0:
			files.append("/" + str(f))
		elif f.is_dir() and f.name in dirs:
			listFiles(f, depth + 1)

listFiles(pathlib.Path(), 0)

x = datetime.datetime.now()
date = x.strftime("%a, %d %b %Y %H:%M:%S GMT")

body_start = """<html>
	<body>\n"""
body_end = """	</body>
</html>"""
body_middle = "		<h1>Index:</h1>\n"

for f in files:
    body_middle = body_middle + f"		<a href={f}> {f} </a><br>"

body = body_start + body_middle + body_end

header = f"""HTTP/1.1 200\r
Content-Length: {len(body.encode("utf-8"))}\r
Content-type: text/html; charset=utf-8\r
Date: {date}\r
Last-Modified: {date}\r
Server: CODAM_WEBSERV\r\n\r"""

print(header)
print(body)
print("\0")

#!/usr/bin/python3

import pathlib, datetime, os, sys

status = 200
message = ""
files = []
def listFiles(path) -> (int, str):
	if path.exists() == False:
		return (404, "Location not found")
	for f in path.iterdir():
		if f.is_file():
			files.append("/" + str(f))
		elif f.is_dir():
			listFiles(f)
	return (200, "Index:")

if os.environ.get("PATH_INFO") != None:
	if os.environ.get("QUERY_STRING") == None:
		status, message = listFiles(pathlib.Path(os.environ.get("PATH_INFO")))
		if status == 200:
			status, message = listFiles(pathlib.Path("./cgi-bin"))
	else:
		key, value = os.environ.get("QUERY_STRING").split("=")
		if value == "cgi-bin":
			status, message = listFiles(pathlib.Path("./cgi-bin"))
		else:
			status, message = listFiles(pathlib.Path(os.environ.get("PATH_INFO").join(value)))
else:
	status = 500
	message = "Internal Server Error"

x = datetime.datetime.now()
date = x.strftime("%a, %d %b %Y %H:%M:%S GMT")

body_start = """<!DOCTYPE html>
<html>
	<body>\n"""
body_end = """	</body>
</html>"""
body_middle = f"		<h1>{message}</h1>\n"

if status == 200:
	for f in files:
		body_middle = body_middle + f"		<a href={f}> {f} </a><br>\n"

body = body_start + body_middle + body_end

header = f"""HTTP/1.1 200\r
Content-Length: {len(body.encode("utf-8"))}\r
Content-type: text/html; charset=utf-8\r
Date: {date}\r
Last-Modified: {date}\r
Server: {os.environ.get("SERVER")}\r\n\r"""

print(header)
print(body)
print("\0")

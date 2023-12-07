#!/usr/bin/python3

import pathlib, datetime, os, sys

status = 200
message = ""
files = []

def listFiles(path) -> (int, str):

	# Check if the path exists
	if path.exists() == False:
		return (404, "Location not found")

	# Loop through the items in the directory
	for f in path.iterdir():
		print(f, file=sys.stderr)
		if f.is_file():
			files.append(str(f))
		elif f.is_dir():
			listFiles(f)

	return (200, "Index:")

def listDirs() -> (int, str):

	# Check if the server sent the root path
	if os.environ.get("PATH_INFO") != None:

		# Check if the server sent the query string
		# else list the files in the root directory
		if os.environ.get("QUERY_STRING") != None:
			value = os.environ.get("QUERY_STRING")
			if "=" in value:
				value = value.split("=")[1]
			if value == "/cgi-bin/":
				return listFiles(pathlib.Path("./cgi-bin"))
			else:
				return listFiles(pathlib.Path(os.environ.get("PATH_INFO")+"/"+value))
		else:
			status , message = listFiles(pathlib.Path(os.environ.get("PATH_INFO")))
			if status == 200:
				return listFiles(pathlib.Path("./cgi-bin"))
			else:
				return (status, message)
	else:
		return (500 , "Internal Server Error")

status, message = listDirs()

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
		if f.startswith("cgi-bin"):
			p = "http://" + os.environ['HOST'] + "/"+ f
		else:
			p = "http://" + os.environ['HOST'] + "/" + f.split("/")[-1]
		body_middle = body_middle + f"		<a href={p}> /{f} </a><br>\n"

body = body_start + body_middle + body_end

header = f"""HTTP/1.1 {status}\r
Content-Length: {len(body.encode("utf-8"))}\r
Content-type: text/html; charset=utf-8\r
Date: {date}\r
Last-Modified: {date}\r
Server: {os.environ.get("SERVER")}\r\n\r"""

print(header)
print(body)
print("\0")

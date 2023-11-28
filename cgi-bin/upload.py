#!/usr/bin/env python3
import datetime, os, cgi, cgitb, sys

cgitb.enable(display=1)
form = cgi.FieldStorage()

def genFilename(filePath) -> str:

	#split the file name and extension
	fileName, fileExtension = os.path.splitext(filePath)

	#add a number to the filename
	index = 1
	filePath = fileName + str(index) + fileExtension

	# Generate a new filename if the file already exists
	while os.path.exists(filePath) ==  True:
		index += 1
		filePath = fileName + str(index) + fileExtension

	return filePath


def uploadFile(form) -> (int, str) :

	# Create uploads folder if it doesn't exist
	if os.path.exists(os.environ.get("PATH_INFO")) == False:
		return (500, "Internal Server Error")
	uploadDir = os.environ.get("PATH_INFO") + "/" + os.environ.get("UPLOAD_DIR")
	if os.path.exists(uploadDir) == False:
		os.mkdir(uploadDir)
	
	# Checks if the file is uploaded
	if form.getvalue('filename'):
		fileitem = form['filename']

		# Test if the file was previous
		fn = os.path.basename(fileitem.filename.replace("\\", "/"))
		filePath = os.path.join(uploadDir, fn)

		if os.path.exists(filePath) == True:
			filePath = genFilename(filePath)

		# Write the file to the uploads folder
		try:
			with open(os.path.join(filePath), 'wb') as f:
				f.write(fileitem.file.read())
			return (201, f"\"{fn}\" uploaded successfully!!!")
		except:
			return (500, "Upload failed!!!")
	else:
		return (500, "No file was uploaded!!!")

# Get the return values from the function
status, message =  uploadFile(form)

# Get the current date and time in readable format
x = datetime.datetime.now()
date = x.strftime("%a, %d %b %Y %H:%M:%S GMT")

		# <link rel="icon" href="data:,">
body = f"""<!DOCTYPE html>
<html>
	<head>
		<title>File Upload</title>
	</head>
	<body>
		<h1>{message}</h1>
	</body>
</html>"""

header = f"""HTTP/1.1 {status}\r
Content-Length: {len(body)}\r
Content-type: text/html; charset=utf-8\r
Connection: close\r
Date: {date}\r
Last-Modified: {date}\r
Connection: close\r
Server: {os.environ.get("SERVER")}\r\n\r"""

print(header)
print(body)
print("\0")

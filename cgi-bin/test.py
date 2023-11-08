#!/usr/bin/env python3

import datetime

x = datetime.datetime.now()
date = x.strftime("%a, %d %b %Y %H:%M:%S GMT")
body = "<html><body><h1>It works!</h1></body></html>"

header = f"HTTP/1.1 200\r\n\
Content-Length: {len(body)}\r\n\
Content-type: text/html; charset=utf-8\r\n\
Date: {date}\r\n\
Last-Modified: {date}\r\n\
Server: CODAM_WEBSERV\r\n\r"

print(header)
print(body)

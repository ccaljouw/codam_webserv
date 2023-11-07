#!/usr/bin/env python3

import datetime

x = datetime.datetime.now()
date = x.strftime("%a, %d %b %Y %H:%M:%S GMT")
body = "<html><body><h1>It works!</h1></body></html>"

print("HTTP/1.1 200\r")
print(f"Content-Length: {len(body)}\r")
print("Content-type: text/html; charset=utf-8\r")
print(f"Date: {date}\r")
print(f"Last-Modified: {date}\r")
print(f"Last-Modified: {date}\r")
print("Server: CODAM_WEBSERV\r")
print("\r")
print(body)

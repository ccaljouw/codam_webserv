#!/usr/bin/python3

# Import modules for CGI handling
import cgi, cgitb

# Create instance of FieldStorage
form = cgi.FieldStorage()

# Get data from fields
first_name = form.getvalue('first_name')
last_name = form.getvalue('last_name')

if first_name is None:
    first_name = "None"
if last_name is None:
    last_name = "None"

print("Content-type:text/html")
print()
print("<html>")
print('<head>')
print("<title>Hello - Second CGI Program</title>")
print('</head>')
print('<body>')
print(f"<h2>Hello {first_name} {last_name}</h2>")
print('</body>')
print('</html>')
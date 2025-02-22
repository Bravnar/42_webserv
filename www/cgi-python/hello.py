#!/usr/bin/python
content = """
<html>
<head>
<title>Hello World - First CGI Program</title>
</head>
<body>
<h2>Hello World! This is my first CGI program</h2>
</body>
</html>
"""
print("Content-Type: text/html\r\n\r\n")
print(f"Content-Length: {len(content)}")
print()
print (content)
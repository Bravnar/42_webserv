#!/usr/bin/python3

import cgi
import os

upload_dir = os.getenv("UPLOAD_DIR", "./www/uploads")
boundary = os.getenv("HTTP_BOUNDARY", "")
print(f"DEBUG: Boundary from env -> {boundary}")

if not os.path.exists(upload_dir):
	os.makedirs(upload_dir)

form = cgi.FieldStorage()
file_item = form["uploaded_file"]

if file_item.filename:
	filename = os.path.basename(file_item.filename)
	file_path = os.path.join(upload_dir, filename)

	with open(file_path, "wb") as f:
		f.write(file_item.file.read())

	print("Content-Type: text/html\r\n")
	print("<html><body>")
	print(f"<h2>File '{filename}' uploaded successfully!</h2>")
	print(f"<p>Saved at: {file_path}</p>")
	print("</body></html>")
else:
	print("Content-Type: text/html\r\n")
	print("<html><body><h2>No file uploaded.</h2></body></html>")
#!/usr/bin/python3

import cgi
import os

# build preresponse
successContent = " \
<!DOCTYPE html> \
<html lang=\"en\"> \
<head> \
	<link rel=\"icon\" href=\"./winnie_python.jpg\"> \
	<link rel=\"stylesheet\" href=\"./upload_assets/style.css\"> \
    <meta charset=\"UTF-8\"> \
    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"> \
    <title>WebservSIR</title> \
</head> \
<body> \
	<section> \
		<div class=\"container\"> \
			<h1>Hunny received!</h1> \
			<p>We received your {filename} hunny</p> \
			<img src=\"./upload_assets/hunny.png\" alt=\"Placeholder Image\"> \
			<hr> \
			<p>Take a look at uploaded files</p> \
			<div class=\"buttons cgi-buttons\"> \
				<a href=\"./\">go back</a> \
				<a href=\"./uploads/\">/uploads</a> \
			</div> \
		</div> \
	</section> \
</body> \
</html> \
"

errorContent = " \
<!DOCTYPE html> \
<html lang=\"en\"> \
<head> \
	<link rel=\"icon\" href=\"/winnie.jpg\"> \
	<link rel=\"stylesheet\" href=\"./upload_assets/style.css\"> \
    <meta charset=\"UTF-8\"> \
    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"> \
    <title>WebservSIR</title> \
</head> \
<body> \
	<section> \
		<div class=\"container\"> \
			<h1>Oh noo!</h1> \
			<p>An error occured while uploading the file!</p> \
			<img src=\"./upload_assets/sad_winnie.jpg\" alt=\"Placeholder Image\"> \
			<hr> \
			<p>Take a look at uploaded files</p> \
			<div class=\"buttons cgi-buttons\"> \
				<a href=\"./\">go back</a> \
				<a href=\"./uploads/\">/uploads</a> \
			</div> \
		</div> \
	</section> \
</body> \
</html> \
"

if os.getenv("REQUEST_METHOD", "") != "POST":
	content = errorContent
	print("Content-Type: text/html")
	print(f"Content-Length: {len(content) + 1}")
	print()
	print(content)
	exit(0)

upload_dir = os.getenv("UPLOAD_DIR", "./www/uploads")
boundary = os.getenv("HTTP_BOUNDARY", "")
# print(f"DEBUG: Boundary from env -> {boundary}")

if not os.path.exists(upload_dir):
	os.makedirs(upload_dir)

try:
	form = cgi.FieldStorage()
	file_item = form["uploaded_file"]
except Exception as e:
	content = errorContent
	print("Content-Type: text/html")
	print(f"Content-Length: {len(content) + 1}")
	print()
	print(content)
	exit(0)

def get_unique_filename(directory, filename):
	base, ext = os.path.splitext(filename)
	counter = 1
	new_filename = filename
	while os.path.exists(os.path.join(directory, new_filename)):
		new_filename = f"{base}_{counter}{ext}"
		counter += 1

	return new_filename

if file_item.filename:
	filename = os.path.basename(file_item.filename)
	filename = get_unique_filename(upload_dir, filename)
	file_path = os.path.join(upload_dir, filename)

	try:
		with open(file_path, "wb") as f:
			f.write(file_item.file.read())
		content = successContent.replace("{filename}", filename)
	except Exception as e:
		if os.path.exists(file_path):
			os.remove(file_path)
		content = errorContent
else:
	print("youas9dasd") 
	content = errorContent

print("Content-Type: text/html")
print(f"Content-Length: {len(content) + 1}")
print()
print(content)
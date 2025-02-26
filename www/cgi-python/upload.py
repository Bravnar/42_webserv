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
		content = f"""
		<!DOCTYPE html>
		<html lang="en">
			<body>
				<h2>File '{filename}' uploaded successfully!</h2>
				<p>Saved at: {file_path}</p>
				<a href="/cgi-python/">Go Back.</a>
			</body>
		</html>
		"""
	except Exception as e:
		if os.path.exists(file_path):
			os.remove(file_path)
		content = f"""
        <!DOCTYPE html>
        <html lang="en">
            <body>
                <h2>Error uploading file!</h2>
                <p>Reason: {str(e)}</p>
                <a href="/cgi-python/">Go Back.</a>
            </body>
        </html>
        """
else:
	content = "<!DOCTYPE html><html lang=\"en\"><body><h2>No file uploaded.</h2></body></html>"

print("Content-Type: text/html")
print(f"Content-Length: {len(content) + 1}")
print()
print(content)
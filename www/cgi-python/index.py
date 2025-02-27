#!/usr/bin/python3

import datetime

now = datetime.datetime.now()
formatted_time = now.strftime("%H:%M")

hour = now.hour 

if 5 <= hour < 12:
	greeting = "Good Morning!"
elif 12 <= hour < 18:
	greeting = "Good Afternoon!"
elif 18 <= hour < 22:
	greeting = "Good Evening!"
else:
    greeting = "Good Night!"

content = f"""
<!DOCTYPE html>
<html lang="en">
<head>
    <link rel="icon" href="./winnie_python.jpg">
	<link rel="stylesheet" href="./style.css">
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>PythonCGI!</title>
</head>
<body>
    <section>
        <div class="container">
            <h1>WebServ-PythonCGI</h1>
			<h2><span style="color: blue;">{greeting}</span> It is now: <span style="color: blue;">{formatted_time}</span>!</h2>
        </div>
        <div class="container">
            <h1>Welcome to Python CGI Page!</h1>
            <img src="./winnie_python.jpg" alt="Image Placeholder">
            <br>
            <p>The main website is at <strong>http://127.0.0.1/</strong></p>
            <p>But this one is at <strong>http://127.0.0.1/cgi-python/index.py</strong>!</p>
            <div class="buttons">
                <a href="/">Go To Main Site</a>
                <a href="/404notfound">Get a 404 Error!</a>                
            </div>
            <div class="buttons cgi-buttons">
                <a href="../cgi-php/">PHP CGI</a>
                <a href="../cgi-bash/">BASH CGI</a>
            </div>
        </div>
		<div class="container">
            <h3>Upload your file!</h3>
			<br>
			<form action="/cgi-python/upload.py" method="post" enctype="multipart/form-data">
                <input type="file" name="uploaded_file">
                <input type="submit" value="Upload">
            </form>
		</div>
    </section>
</body>
</html>
"""
print("Content-Type: text/html")
print(f"Content-Length: {len(content) + 1}")
print()
print(content)

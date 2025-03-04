#!/usr/bin/php-cgi

<?php

$day_of_week = date("l");

$day_message = "$day_of_week!";

$content = "
<!DOCTYPE html>
<html lang=\"en\">
<head>
    <link rel=\"icon\" href=\"./winnie_php.jpg\">
	<link rel=\"stylesheet\" href=\"./style.css\">
    <meta charset=\"UTF-8\">
    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">
    <title>PhP-CGI!</title>
</head>
<body>
    <section>
        <div class=\"container\">
            <h1>WebServ-PHP-CGI</h1>
            <h2>Happy <span style=\"color: blue;\">$day_message</span> Hope you're having a great day!</h2>
        </div>
        <div class=\"container\">
            <h1>Welcome to Python CGI Page!</h1>
            <img src=\"./winnie_php.jpg\" alt=\"Image Placeholder\">
            <br>
            <p>The main website is at <strong>http://127.0.0.1/</strong></p>
            <p>But this one is at <strong>http://127.0.0.1/cgi-php/index.php</strong>!</p>
            <div class=\"buttons\">
                <a href=\"/\">Go To Main Site</a>
                <a href=\"/404notfound\">Get a 404 Error!</a>                
            </div>
            <div class=\"buttons cgi-buttons\">
                <a href=\"../cgi-python/\">PYTHON CGI</a>
                <a href=\"../cgi-bash/\">BASH CGI</a>
                <a href=\"../cgi-php/calculator.html\">CGI Calculator!</a>
            </div>
        </div>
    </section>
    <section>
        <div class=\"container\">
            <h3>Upload your file!</h3>
			<br>
			<form action=\"/cgi-php/upload.php\" method=\"post\" enctype=\"multipart/form-data\">
                <input type=\"file\" name=\"uploaded_file\">
                <input type=\"submit\" value=\"Upload\">
            </form>
		</div>
    </section>
</body>
</html>
";

header("Content-Type: text/html");
header("Content-Length: " . strlen($content));

echo $content;
?>
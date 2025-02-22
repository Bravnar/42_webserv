#!/usr/bin/php-cgi
<?php
$content = "
<html>
<head>
<title>Hello World - First CGI Program</title>
</head>
<body>
<h2>Hello World! This is my first PHP CGI program</h2>
</body>
</html>
";

header("Content-Type: text/html");
header("Content-Length: " . strlen($content));

echo $content;
?>
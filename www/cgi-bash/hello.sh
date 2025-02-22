#!/bin/bash

CONTENT="
<html>
<head>
    <title>Hello World - Bash CGI</title>
</head>
<body>
    <h2>Hello World! This is my first Bash CGI script!</h2>
</body>
</html>
"
CONTENT_LENGTH=$(echo -n "$CONTENT" | wc -c)

echo "Content-Type: text/html"
echo "Content-Length: $CONTENT_LENGTH"
echo ""

echo "$CONTENT"
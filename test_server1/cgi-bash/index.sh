#!/bin/bash

# Random quotes array
quotes=("Keep pushing forward!" 
        "Stay hungry, stay foolish." 
        "Code like there's no tomorrow." 
        "Simplicity is the soul of efficiency." 
        "Hello from Bash CGI!")

# Select random quote
random_quote=${quotes[$RANDOM % ${#quotes[@]}]}

# Generate the HTML content
CONTENT=$(cat <<EOF
<!DOCTYPE html>
<html lang="en">
<head>
    <link rel="icon" href="./winnie_bash.jpg">
	<link rel="stylesheet" href="./style.css">
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Bash CGI!</title>
</head>
<body>
    <section>
        <div class="container">
            <h1>WebServ-BashCGI</h1>
            <h2>Quote of the moment: <span style="color: blue;">$random_quote</span>!</h2>
        </div>
        <div class="container">
            <h1>Welcome to Bash CGI Page!</h1>
            <img src="./winnie_bash.jpg" alt="Image Placeholder">
            <br>
            <p>The main website is at <strong>http://127.0.0.1/</strong></p>
            <p>But this one is at <strong>http://127.0.0.1/cgi-bash/index.sh</strong>!</p>
            <div class="buttons">
                <a href="/">Go To Main Site</a>
                <a href="/404notfound">Get a 404 Error!</a>                
            </div>
            <div class="buttons cgi-buttons">
                <a href="../cgi-python/">PYTHON CGI</a>
                <a href="../cgi-php/">PHP CGI</a>
            </div>
        </div>
    </section>
</body>
</html>
EOF
)

# Calculate content length
CONTENT_LENGTH=$(echo -n "$CONTENT" | wc -c)

# Output HTTP headers and the content
echo "Content-Type: text/html"
echo "Content-Length: $CONTENT_LENGTH"
echo ""
echo "$CONTENT"

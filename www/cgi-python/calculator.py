#!/usr/bin/python

# Import modules for CGI handling 
import os, sys, json
import cgi, cgitb
from urllib.parse import quote, unquote

# Create instance of FieldStorage 
form = cgi.FieldStorage() 

histo = []

if 'HTTP_COOKIE' in os.environ.keys():
   for cookie in os.environ['HTTP_COOKIE'].split(';'):

      (key, value) = cookie.split('=')
      key = key.strip()

      if key == "histo":
         histo = json.loads(unquote(value))

if "REQUEST_METHOD" in os.environ:
   method = os.environ["REQUEST_METHOD"]
else:
   method = "GET"



res = None
if (form.getvalue('nbr1') != None and form.getvalue('nbr2') != None and form.getvalue('operation') != None):

   nbr1 = int(form.getvalue('nbr1'))
   nbr2 = int(form.getvalue('nbr2'))
   operation = form.getvalue('operation')

   if operation == '+':
      res = nbr1 + nbr2

   elif operation == '-':
      res = nbr1 - nbr2

   elif operation == '*':
      res = nbr1 * nbr2

   elif operation == '/':
      if (nbr2 != 0):
         res = nbr1 / nbr2
      else:
         res = "impossible"

   else:
      res = "invalid operation"



   histo.append(f"Python {method} : {nbr1} {operation} {nbr2} is {res}")
   histo = histo[-10:]

   sendhisto = quote(json.dumps(histo))
   # print(histo, file=sys.stderr)
   print (f"Set-Cookie: histo={sendhisto}; expires=Fri, 03 Mar 2023 10:24:45 GMT; Max-Age=2592000; path=/")


print ("Content-Type: text/html")
print()

print("""
<!DOCTYPE html>
<html lang="en">
<head>
    <link rel="icon" href="./winnie_php.jpg">
    <link rel="stylesheet" href="./style.css">
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Python CGI!</title>
</head>
<body>
    <section>
        <div class="container">
            <h1>CGI-Python Calculator</h1>
            <h2>Hope you're having a great day!</h2>
        </div>
        <div class="container">
            <h1>Welcome to Python CGI Page!</h1>
            <div style="margin-top: 0px;" class="container">
                <h2>Results</h2>
""")

if res is not None:
    print(f"<p>Calcul made with {method}</p>")
    print(f"<p>Result of {nbr1} {operation} {nbr2} is {res} !!!!</p>")

print("""
                <br><br>
                <h2>Historique</h2>
""")

for i in histo:
    print(f"<p>{i}</p>")

print("""
            </div>
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
                <a href="../cgi-python/calculator.html">CGI Calculator!</a>
            </div>
        </div>
    </section>
</body>
</html>
""")

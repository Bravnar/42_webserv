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


print ("")
print ("<html>")
print ("<head>")
print ("<title>CALCULATOR</title>")
print ("</head>")
print ("<body>")

if (res != None):
   print (f"calcul made with {method}<br>")
   print (f"result of {nbr1} {operation} {nbr2} is {res} !!!! <br>")
print (f"<br><br>")


print("Historique<br>")
for i in histo:
   print(i)
   print("<br>")


print ("</body>")
print ("</html>")
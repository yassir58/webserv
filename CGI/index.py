#!/usr/bin/env python3

import cgi, cgitb
cgitb.enable()

postData = cgi.FieldStorage()

print('Content-Type: text/html') # HTML is following
print('');

username = postData["username"].value;
password = postData["password"].value;

print(username);
print(password);
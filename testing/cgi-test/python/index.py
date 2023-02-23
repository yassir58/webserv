#!/usr/bin/env python3

import cgi, cgitb
cgitb.enable()

postData = cgi.FieldStorage()

name = postData["name"].value;
name);
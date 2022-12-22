#!/bin/bash

HOST_NAME="127.0.0.1"
HOST_PORT="8080"

if ( (exec 3<>/dev/tcp/${HOST_NAME}/${HOST_PORT}) 2> /dev/null); then
echo -e "PORT: ${HOST_PORT} | ON"
else
cd /home/yassir/Documents/CURSES/WEBSERV && `./server`
fi


echo "\e[0;33m testing servers on ports 8080 1200 5000 ..."

curl http://192.168.1.140:1200?[1-1024] ; curl http://192.168.1.140:8080?[1-1024] ; curl http://192.168.1.140:5000?[1-1024]

exit;

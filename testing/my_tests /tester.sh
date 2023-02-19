#!/bin/bash

Black='\033[0;30m'        # Black
Red='\033[0;31m'          # Red
Green='\033[0;32m'        # Green
Yellow='\033[0;33m'       # Yellow
Blue='\033[0;34m'         # Blue
Purple='\033[0;35m'       # Purple
Cyan='\033[0;36m'         # Cyan
White='\033[0;37m'        # White


HOST_NAME="127.0.0.1"
HOST_PORT="8080"

ENDPOINT="http://localhost:8080"
FORBIDDEN_PATH="/test/forbidden/forbidden.html"
FORBIDDEN_FILE="/Users/yelatman/Documents/webserv/www/test/forbidden/forbidden.html"
FILE_PATH=""/Users/yelatman/Documents/webserv/www/test/srcs/index.html""
NOT_ALLOWED_PATH="/getNotAllowed/"
REDIR_PATH="/redir/"
DIFFERENT_ROOT="/differenteRoot"
SERVNAME_TEST="/"
SERVNAME_PATH="/Users/yelatman/Documents/webserv/www/test/servNameTest/serverName.html"
NORMAL_POST_PATH="/upload/test.txt"
POST_DEST_PATH="/Users/yelatman/Documents/webserv/www/upload/uploadTest/test.txt"
NOT_ALLOWED_POST="/upload/notAllowed"
FORBIDDEN_POST="/upload/forbidden/forbidden.txt"
PAYLOAD_TOO_LARGE="/upload/toLarge"
URI_TOO_LONG=""
EXEC_PATH="/Users/yelatman/Documents/webserv/webserv"
CONF_PATH="/Users/yelatman/Documents/webserv/testing/configs/test.conf"
ERROR_LOG="/Users/yelatman/Documents/webserv/error.log"
SERVNAME_REQ="GET / HTTP/1.1\r\nHost: servName\r\nUser-Agent: MyHTTPClient/1.0\r\nAccept: text/html\r\nConnection: keep-alive\r\n\r\n"
POST_REQ="HTTP/1.1\r\nHost: localhost:8080\r\nUser-Agent: MyHTTPClient/1.0\r\nContent-Type: text/plain\r\nConnection: keep-alive\r\n\r\n"
POST_PATH="/Users/yelatman/Documents/webserv/www/post/test.txt"
POST_ROUTE="/post/test.txt"
LARGE_RESOURCE="/Users/yelatman/goinfre/webserv_test/capybara.mp4"

if [ -z "$EXEC_PATH" ] ; then
	echo "please provide a path for webserv executable : " && read EXEC_PATH
fi
if [ -z "$CONF_PATH" ] ; then
	echo "please provide a path for config file :" && read CONF_PATH
fi

terminateServer ()
{
	echo -e "$Red terminating server $White "
	killall -9 webserv > /dev/null 2>&1
}


testGET ()
{
	if [ "$4" = "BAD REQUEST" ]; then 
		echo "Invalid http request" | nc localhost 8080 > HEADERS;
		status_code=$(cat HEADERS | grep HTTP | cut -d' ' -f2)
	else
		if [ "$4" = "DIFFERENT SERVER NAME" ]; then
			echo -e $SERVNAME_REQ | nc localhost 8080 > response.txt;
			cat response | grep "<" > test ; cat response.txt > HEADERS ; rm -rf response.txt
			status_message=$(cat HEADERS | grep "HTTP" | cut -d' ' -f3)
			status_code=$(cat HEADERS | grep "HTTP" | cut -d' ' -f2)
		else
			if [ "$4" == "FORBIDDEN REQUEST" ]; then
				chmod 000 $FORBIDDEN_FILE
			fi
			curl -v $ENDPOINT$1 -o test &> HEADERS
			status_message=$(cat HEADERS | grep "<" | grep "HTTP" | cut -d' ' -f4)
			status_code=$(cat HEADERS | grep "<" | grep "HTTP" | cut -d' ' -f3)
		fi
	fi
	echo -e "testing $4 on $Yellow $ENDPOINT$1 $White"
	echo -n -e "status code test :"
	if [ "$3" == "$status_code" ] ; then
		echo -e "$Green    succeded ✓ $White"
	else
		echo -e "$Red  failed X $White"
	fi
	echo -n "response diff test : "
	diff test $3 2>/dev/null > response.diff
	if [ -s response.diff ] ; then
		echo -e "$Red failed X $white"
	else
		echo -e "$Green succeded ✓ $White"
	fi
	if [ "$4" == "FORBIDDEN REQUEST" ]; then
				chmod +rw $FORBIDDEN_FILE
			fi
	rm -rf HEADERS test nohup.out response.diff
}


testPOST ()
{
	echo "" >> $POST_PATH
	POST_BODY=$(for i in {1..1024}; do echo -n "test" >> testFile; done)
	if [ "$3" == "UPLOAD LARGE RESOURCE" ]; then
		curl -v -d @$LARGE_RESOURCE http://localhost:8080$1 &> HEADERS
	elif [ "$3" == "CLIENT BODY TOO LARGE" ]; then
		curl -v -d @testFile http://localhost:8081$1 &> HEADERS
	else
		curl -v -d @testFile http://localhost:8080$1 &> HEADERS
	fi
	status_message=$(cat HEADERS | grep "<" | grep "HTTP" | cut -d' ' -f4)
	status_code=$(cat HEADERS | grep "<" | grep "HTTP" | cut -d' ' -f3)	
	echo -e "testing $3 on $Yellow $ENDPOINT$1 $White"
	echo -n -e "status code test  : "

	if [ "$status_code" = "$2" ]; then
		echo -e "$Green  succeded ✓ $White"
	else
		echo -e "$Red  failed X $White"
	fi
	if [ -f "$5" ]; then
	if [ "$3" == "UPLOAD LARGE RESOURCE" ]; then
			diff $4 $5 2>/dev/null > response.diff
		else
			diff $4 testFile 2>/dev/null > response.diff
		fi
		echo -n "response diff test : "
		if [ -s response.diff ]; then 
			 echo -e "$Red failed X $White"
		else
			echo -e "$Green succeded ✓ $White"
		fi
		rm -rf testFile  response.diff
	fi
	rm -rf HEADERS testFile
}



stressTesting ()
{
	echo -e $1
	for i in {1..1024}; do echo -e $2  | nc localhost 8080 > /dev/null; done
	PID=$(pgrep webserv)
	if [  -z "$PID" ]; then
		echo -e "$Red test failed X $White"
	else
		echo -e "$Green test succeded ✓ $White"
	fi
	
}

exec $EXEC_PATH $CONF_PATH 1>/dev/null 2>$ERROR_LOG  &
PID=$(ps a | grep {$EXEC_PATH} |tr -s ' ' | cut -d ' ' -f1)
echo -e " server started on the background :  pid $Green $PID $White"
echo -e " server listening on port $Cyan $HOST_PORT $White"
sleep 2
testGET "/" $FILE_PATH "200" "SIMPLE GET REQUEST"
testGET $REDIR_PATH $FILE_PATH "301" "REDIRECTION"
testGET "/notFound" "" "404" "RESOURCE NOT FOUND"
testGET "" "" "400" "BAD REQUEST"
testGET $FORBIDDEN_PATH "" "403" "FORBIDDEN REQUEST"
testGET $NOT_ALLOWED_PATH "" "405" "NOT ALLOWED GET REQUEST"
testGET $SERVNAME_TEST $SERVNAME_PATH "200" "DIFFERENT SERVER NAME"
testPOST $POST_ROUTE "200" "SIMPLE POST REQUEST" $POST_PATH 
testPOST "/notAllowedUpload" "405" "NOT ALLOWED POST REQUEST"  ""
testPOST "/forbiddenUpload/file.txt"  "403" "FORBIDDEN POST REQUEST" ""
testPOST "/upload/capybara.mp4" "201" "UPLOAD LARGE RESOURCE" $LARGE_RESOURCE "/Users/yelatman/Documents/webserv/www/test/upload/capybara.mp4"
testPOST "/toLarge/test.txt"  "413" "CLIENT BODY TOO LARGE"
# stressTesting "stress test with bad start line :" "GET\r\n\r\n"
# stressTesting "stress with bad method :" "GEt / HTTP/1.1\r\nHost:serv\r\n\r\n"
# stressTesting "stress test with missing headers :" "GET / HTTP/1.1\r\n\r\n\r\n"
# stressTesting "stress test with valid headers :" "GET / HTTP/1.1\r\nHost:localhost:8080\r\n\r\n"
trap terminateServer EXIT

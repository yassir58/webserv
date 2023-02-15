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
NOT_FOUND_PATH="/test/notFound"
FORBIDDEN_PATH="/test/forbidden/forbidden.html"
FORBIDDEN_FILE="/Users/yelatman/Documents/webserv/www/test/forbidden/forbidden.html"
FILE_PATH=""/Users/yelatman/Documents/webserv/www/test/srcs/index.html""
NOT_ALLOWED_PATH="/test/getNotAllowed/"
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
	if [ "$4" = "BAD RQUEST" ]; then 
		echo "Invalid http request" | nc localhost 8080 > HEADERS;
		status_code=$(cat HEADERS | grep HTTP | cut -d' ' -f2)
	else
		if [ "$4" = "DIFFERENT SERVER NAME" ]; then
			echo -e $SERVNAME_REQ | nc localhost 8080 > response;
			cat response | grep "<" > test ; cat response > HEADERS ; rm -rf response 
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
	echo -e " testing $4 on $Yellow $ENDPOINT$1 $White"
	echo -n " matching status codes  : "
	if [ "$3" == "$status_code" ] ; then
		echo -e "$Green status code test succeded ✓ $White"
	else
		echo -e "$Red status code test failed X $White"
	fi
	echo -n " matching response body : "
	diff test $3 2>/dev/null > response.diff
	if [ -s response.diff ] ; then
		echo -e "$Red body diff test failed X $white"
	else
		echo -e "$Green body diff test succeded ✓ $White"
	fi
	if [ "$4" == "FORBIDDEN REQUEST" ]; then
				chmod +rw $FORBIDDEN_FILE
			fi
	rm -rf HEADERS test nohup.out reponse.diff
}


testPOST ()
{
	POST_BODY=$(for i in {1..1024}; do echo -n "test" >> testFile; done)
	curl -v -d @testFile http://localhost:8080/upload/$5 &> HEADERS
	status_message=$(cat HEADERS | grep "<" | grep "HTTP" | cut -d' ' -f4)
	status_code=$(cat HEADERS | grep "<" | grep "HTTP" | cut -d' ' -f3)

	
	echo -e " testing $3 on $Yellow $ENDPOINT$1 $White"
	echo -n " matching status codes  : "
	
	if [ "$status_code" = "$2" ]; then
		echo -e "$Green status code test succeded ✓ $White"
	else
		echo -e "$Red status code test failed X $White"
	fi
	if [ -f "$4" ]; then
		diff $5 testFile 2>/dev/null > response.diff
		echo -n " matching response body : "
		if [ -s response.diff ]; then 
			 echo -e "$Red diff test failed X $White"
		else
			echo -e "$Green diff test succeded ✓ $White"
		fi
		rm -rf testFile $5 response.diff
	fi
	rm -rf HEADERS testFile
}

exec $EXEC_PATH $CONF_PATH 1>/dev/null 2>$ERROR_LOG  &
PID=$(ps a | grep {$EXEC_PATH} |tr -s ' ' | cut -d ' ' -f1)
echo -e " server started on the background :  pid $Green $PID $White"
echo -e " server listening on port $Cyan $HOST_PORT $White"
sleep 2
testGET "/" $FILE_PATH "200" "SIMPLE GET REQUEST"
testGET $EDIR_PATH $FILE_PATH "200" "REDIRECTION"
testGET $NOT_FOUND_PATH "" "404" "RESOURCE NOT FOUND"
testGET "" "" "400" "BAD REQUEST"
testGET $FORBIDDEN_PATH "" "403" "FORBIDDEN REQUEST"
testGET $NOT_ALLOWED_PATH "" "405" "NOT ALLOWED GET REQUEST"
testGET $SERVNAME_TEST $SERVNAME_PATH "200" "DIFFERENT SERVER NAME"
testPOST $NORMAL_POST_PATH "201" "simple upload" "/Users/yelatman/Documents/webserv/www/test/upload/test.txt" "test.txt"
# testPOST $NOT_ALLOWED_POST  "405" "not allowed post method" "" "/notAllowed"
# testPOST $FORBIDDEN_POST "" "403" "forbidden post method" ""
# testPOST "toLarge"  "413" "l"
trap terminateServer EXIT

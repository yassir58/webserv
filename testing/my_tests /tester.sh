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
FORBIDDEN_PATH="/forbidden"
NOT_ALLOWED_PATH="/getNotAllowed"
NORMAL_PATH="/test"
DIFFERENT_ROOT="/differenteRoot"
SERVNAME_TEST="/servNameTest"
SERVNAME_PATH="/Users/yelatman/Documents/webserv/www/servNameTest/serverName.html"
NORMAL_POST_PATH=""
NOT_ALLOWED_POST=""
FORBIDDEN_POST=""
PAYLOAD_TOO_LARGE=""
URI_TOO_LONG=""
EXEC_PATH="/Users/yelatman/Documents/webserv/webserv"
CONF_PATH="/Users/yelatman/Documents/webserv/testing/configs/test.conf"
ERROR_LOG="/Users/yelatman/Documents/webserv/error.log"

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
	if [ "$4" = "bad request" ]; then 
		echo "Invalid http request" | nc localhost 8080 > HEADERS;
		status_code=$(cat HEADERS | grep HTTP | cut -d' ' -f2)
	else
		if [ "$4" = "server name" ]; then
			curl -v -H "Host : servName" $ENDPOINT$1 -o test.html &> HEADERS
		else
			curl -v $ENDPOINT$1 -o test.html &> HEADERS
		fi
		status_message=$(cat HEADERS | grep "<" | grep "HTTP" | cut -d' ' -f4)
		status_code=$(cat HEADERS | grep "<" | grep "HTTP" | cut -d' ' -f3)
		echo -e "status code : $status_code status message : $status_message"
	fi
	echo -e " testing $4 on $Yellow $ENDPOINT$1 $White"
	echo -n " matching status codes  : "
	if [ "$3" == "$status_code" ] ; then
		echo -e "$Green status code test succeded ✓ $White"
	else
		echo -e "$Red status code test failed X $White"
	fi
	echo -n " matching response body : "
	diff test.html 2>/dev/null > response.diff
	if [ -s response.diff ] ; then
		echo -e "$Red diff test failed X $white"
	else
		echo -e "$Green diff test succeded ✓ $White"
	fi
	
	# rm -rf HEADERS test.html nohup.out reponse.diff
}


testPOST ()
{
	echo "test" >> testFile
	curl -v $ENDPOINT$NORMAL_POST_PATH -d @testFile -H "Content-Type: application/javascript" &> HEADERS
}

exec $EXEC_PATH $CONF_PATH 1>/dev/null 2>$ERROR_LOG  &
PID=$(ps a | grep {$EXEC_PATH} |tr -s ' ' | cut -d ' ' -f1)
echo -e " server started on the background :  pid $Green $PID $White"
echo -e " server listening on port $Yellow $HOST_PORT $White"
sleep 2
testGET $NORMAL_PATH "/Users/yelatman/Documents/webserv/www/test/test.html" "200" "simple GET request"
# testGET "/" "../../www/index.html" "200" "simple GET request"
testGET $NOT_FOUND_PATH "" "404" "resource not FOUND"
testGET "" "" "501" "bad request"
testGET $FORBIDDEN_PATH "" "403" "forbidden operation"
testGET $NOT_ALLOWED_PATH "" "405" "not allowed GET method"
testGET $SERVNAME_TEST $SERVNAME_PATH "200" "server name" 

trap terminateServer EXIT
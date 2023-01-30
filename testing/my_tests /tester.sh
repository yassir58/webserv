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
EXEC_PATH="./../../webserv"
CONF_PATH="../configs/test.conf"
ENDPOINT="http://localhost:8080"
NOT_FOUND_PATH="/test/notFound"
FORBIDDEN_PATH="/test/forbidden.html"
NOT_ALLOWED_PATH="/getNotAllowed"
NORMAL_POST_PATH=""
NOT_ALLOWED_POST=""
FORBIDDEN_POST=""
PAYLOAD_TOO_LARGE=""
URI_TOO_LONG=""



terminateServer ()
{
	echo -e "$Red terminating server $White "
	killall -9 webserv > /dev/null 2>&1
}


testGET ()
{
	if [ $1 == "bad request" ]; then 
		curl -H "Invalid-Header : value" $ENDPOINT
	fi
	curl -v $ENDPOINT$1 -o test.html &> HEADERS
	status_message=$(cat HEADERS | grep "<" | grep "HTTP" | cut -d' ' -f4)
	status_code=$(cat HEADERS | grep "<" | grep "HTTP" | cut -d' ' -f3)

	echo -e " testing $4 on $Yellow $ENDPOINT$1 $White"
	echo " ------> matching status codes"
	if [ $3 == $status_code ] ; then
		echo -e "$Green status code test succeded ✓ $White"
	else
		echo -e "$Red status code test failed X $White"
	fi
	echo " ------> matching response body"
	diff test.html 1&> /dev/null $2 > response.diff
	if [ -s response.diff ] ; then
		echo -e "$Red diff test failed X $white"
	else
		echo -e "$Green diff test succeded ✓ $White"
	fi
	rm -rf HEADERS test.html nohup.out reponse.diff
}


testPOST ()
{
	echo "test" >> testFile
	curl -v $ENDPOINT$NORMAL_POST_PATH -d @testFile -H "Content-Type: application/javascript" &> HEADERS
}


nohup $EXEC_PATH $CONF_PATH > /dev/null &
PID=$(ps a | grep {$EXEC_PATH} |tr -s ' ' | cut -d ' ' -f1)
echo -e " server started on the background :  pid $Green $PID $White"
echo -e " server listening on port $Yellow $HOST_PORT $White"
sleep 2
testGET "/" "./../../www/test.html" "200" "simple GET request"
# testGET $NOT_FOUND_PATH "" "404" "testing resource not FOUND"
# testGET "bad request" "" "400" "testing BAD REQUEST"
# testGet $FORBIDDEN_PATH "" "403" "testing forbidden operation"
# testGet $NOT_ALLOWED_PATH "" "405" "testing not allowed GET method"

trap terminateServer EXIT
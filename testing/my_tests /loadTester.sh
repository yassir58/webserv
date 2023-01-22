#! /bin/bash


Black='\033[0;30m'        # Black
Red='\033[0;31m'          # Red
Green='\033[0;32m'        # Green
Yellow='\033[0;33m'       # Yellow
Blue='\033[0;34m'         # Blue
Purple='\033[0;35m'       # Purple
Cyan='\033[0;36m'         # Cyan
White='\033[0;37m'        # White

read PORT

N=0
AVL=0
TEST_NUMBER=5
NET_INTERFACE=$(ifconfig | grep -w inet | awk 'NR == 2 {print $0}' | cut -d' ' -f2)

echo -e "testing load  interface : $Cyan  $NET_INTERFACE $White port : $Green $PORT $White"


testLoad ()
{

	AVL=$(siege -b -t10s -c$2  http://$NET_INTERFACE:$PORT > siege.out 2>&1 ; cat siege.out | grep  Availability: | sed 's/[^0-9.]*//g') 
	RS=$(cat siege.out | grep "Response time");
	echo -e "server availability under $Cyan $2 $White conccurnet users  in test$Yellow $1 $Green [ Availabilty = $AVL ] $White"
	echo -e "$Yellow $RS"
	if [ $AVL == '100.00' ] 
		then 
			echo -e "$Green test passed ✓ $White"
	else
		echo -e "$Red test failed  X $White"
	fi

}

for i in {1..5}
	do
		testLoad $i $(($i * 10))
	done

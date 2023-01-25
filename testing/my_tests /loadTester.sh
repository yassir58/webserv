#! /bin/bash


Black='\033[0;30m'        # Black
Red='\033[0;31m'          # Red
Green='\033[0;32m'        # Green
Yellow='\033[0;33m'       # Yellow
Blue='\033[0;34m'         # Blue
Purple='\033[0;35m'       # Purple
Cyan='\033[0;36m'         # Cyan
White='\033[0;37m'        # White


TEST_NUMBER=5
NET_INTERFACE=$(ifconfig | grep -w inet | awk 'NR == 2 {print $0}' | cut -d' ' -f2)
ENDPOINTS=("localhost:8080" "10.11.8.2:1337"  "10.11.8.2:4512")


echo $SERVERS



testLoad ()
{

	AVL=$(siege -b -t10s -c$2  http://$3 > siege.out 2>&1 ; cat siege.out | grep  Availability: | sed 's/[^0-9.]*//g') 
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

testInterface ()
{
	echo -e "testing load $Cyan $1 $White"
	for i in {1..5}
		do
			testLoad $i $(($i * 10)) $1
		done
}


for i in ${ENDPOINTS[@]}; do
	testInterface $i
done

rm -rf siege.out
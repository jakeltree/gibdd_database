#!/bin/bash
path="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
echo "Client2 scenario.sh was launched. pathh is ${path}"

time1=1
time2=3
time3=1
# time for sleep

request1="select FIO=Юлия_Горелова_Геннадиевна end"
request2="select SIGN=У470РУ15 end"
request3="select FIO=Виктория_Крылова_Платоновна SIGN=В881ТХ45 end"

echo $request1 > $path/requests.txt
$path/./cli_tcp
sleep $time1
mv $path/selection_client.csv $path/data1.csv

echo $request2 > $path/requests.txt
$path/./cli_tcp
sleep $time2
mv $path/selection_client.csv $path/data2.csv

echo $request3 > $path/requests.txt
$path/./cli_tcp
sleep $time3
mv $path/selection_client.csv $path/data3.csv

rm requests.txt
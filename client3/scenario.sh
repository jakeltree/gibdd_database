#!/bin/bash
path="/home/littytree/Documents/MSU/ЭВМ/gibdd_database/client3"

time1=0.5
time2=2
time3=5
# time for sleep

request1="select FIO=Виктория_Крылова_Платоновна end"
request2="select FIO=Серафима_Лаврентьева_Романовна end"
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
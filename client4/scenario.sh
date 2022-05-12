#!/bin/bash
path="/home/littytree/Documents/MSU/ЭВМ/gibdd_database/client4"

time1=3
time2=1
time3=1
# time for sleep

request1="select SIGN=У060ВС86 end"
request2="select SIGN=Х690НР91 end"
request3="select SIGN=С547НР18 end"

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
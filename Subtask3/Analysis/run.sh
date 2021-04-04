#!/bin/bash
METHOD='method2'
for i in 1 2 3 4 5 6 7 8 9
do
echo "Running test number $i"
time ./$METHOD.o firsthalf.mp4 $i
mv output_"$METHOD"_"$i".csv Outputs_Method3
sleep 30s
done
#!/bin/bash
for i in 1 2 3 4 5 6 7 8 9
do
echo "Running test number $i"
time ./method3.o firsthalf.mp4 $i
mv output_method3_$i.csv Outputs_Method3
sleep 30s
done
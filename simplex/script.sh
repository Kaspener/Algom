#!/bin/bash

for ((i = 1; i <= 6; i++))
do
    ./main "f${i}.txt" > "f${i}.out"
done

for ((i = 1; i <= 6; i++))
do
    ./main "d${i}.txt" > "d${i}.out"
done

for ((i = 1; i <= 8; i++))
do
    ./main "m${i}.txt" > "m${i}.out"
done

for ((i = 1; i <= 4; i++))
do
    ./main "p${i}.txt" > "p${i}.out"
done
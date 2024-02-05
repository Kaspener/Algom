#!/bin/bash

for number in {1..6}; do
    ./main "${number}.txt" > "${number}.out"
    echo "${number} txt to out"
done

for number in {21..30}; do
    ./main "${number}.txt" > "${number}.out"
    echo "${number} txt to out"
done
#!/bin/bash

#gcc -Wall -Werror -std=gnu11 -O2 -g3 -lm main.c -o main
gcc -Wall -Werror -std=gnu11 -O2 -g3 -fsanitize=address -lm main.c -o main

var="Test/Open_Test/open_"

if [ -z "$1" ]
then
    for i in `seq 1 111`;
    do
        echo "Test $i"
        IN="${var}${i}.txt"
        OUT="${var}${i}.output.txt"
        ./main <$IN >"my_out.txt"
        diff "./my_out.txt" ./$OUT
        #read -n1
    done
else
    IN="${var}$1.txt"
    OUT="${var}$1.output.txt"
    ./main <$IN >"my_out.txt"
    read -n1;
    diff "./my_out.txt" ./$OUT
fi
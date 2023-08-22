#!/bin/bash

gcc -Wall -Werror -std=gnu11 -O2 -g3 -lm main.c -o main

var="Test/Open_Test/open_"

for i in `seq 1 111`;
    do
        echo "Test $i"
        IN="${var}${i}.txt"
        OUT="${var}${i}.output.txt"

        valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./main <$IN >"my_out.txt"
        #read -n1
        sleep 1
        diff "./my_out.txt" ./$OUT
    done
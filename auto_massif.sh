#!/bin/bash


rm massif.out.*
gcc -Wall -Werror -std=gnu11 -O2 -g3 -lm main.c -o main
valgrind --tool=massif ./main <"Test/Open_Test/open_${1}.txt" >"my_out.txt"
massif-visualizer massif.out.*
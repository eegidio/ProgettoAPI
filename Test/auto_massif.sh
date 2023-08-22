#!/bin/bash


rm massif.out.*
gcc -Wall -Werror -std=gnu11 -O2 -g3 -lm main.c -o main
valgrind --tool=massif ./main <"${1}" >"my_out.txt"
massif-visualizer massif.out.*
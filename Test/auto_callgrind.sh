#!/bin/bash


rm callgrind.out.*
gcc -Wall -Werror -std=gnu11 -O2 -g3 -lm main.c -o main
valgrind --tool=callgrind ./main <"${1}" >"my_out.txt"
kcachegrind callgrind.out.*
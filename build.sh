#!/bin/bash

gcc -c -std=c99 -Wall -Werror -pedantic -o player.o player.c && gcc -Wall -Werror -pedantic -o pacman player.o pacman.o -lm

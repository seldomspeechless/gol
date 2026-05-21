#/!usr/bin/env sh

gcc main.c -o app $(pkg-config --cflags --libs raylib) && ./app

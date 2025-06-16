#!bin/bash

gcc main.c src/dns_config.c -I include/ -ljansson -o server.exe
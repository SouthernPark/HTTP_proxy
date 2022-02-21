#!/bin/bash

cd /code/

make clean
make clean_test

make

./Daemon

while true
do 
    sleep 1
done
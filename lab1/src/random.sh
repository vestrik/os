#!/bin/bash 


for (( i=0; i < 150; i++ )) 
do 
od -vAn -N1 -td1 < /dev/random >> numbers.txt 
done
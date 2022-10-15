#!/bin/bash 

i=1
IFS=' '

while [ $i -le $# ]         #for each argument
do
    s=0
    for file in ./workerdir/*       #for each file
    do
        while read -r line;     #read line by line the file
        do
            arr=($line);        #split line
            if [[ "${arr[0]}" == *"${!i}" ]]   #argument exists at the end of the adress
                then s=$(( $s + ${arr[1]} ));
            fi
        done < "$file"
    done
    echo "Total number of appearances of ${!i} is:" $s;
    i=$(($i+1))                 #next argument
done
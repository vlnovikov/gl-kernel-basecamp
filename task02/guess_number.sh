#!/bin/bash

function generate_number() {
    return $(($1 + $RANDOM % $2))
}

function compare_numbers() {
    if [[ $2 < $1 ]];
    then
        echo Y is less than X
    elif [[ $2 > $1 ]]
    then
        echo Y is greater than X
    else
        echo Y is equal X
    fi
}

X=$1
limit=100
attempts=10

if [[ -nz $2 ]];
then
    limit=$2
fi

if [[ -nz $3 ]];
then
    attempts=$3
fi

if [[ -nz $X ]];
then
    compare_numbers $X $Y
    let attempts=$attempts-1
fi

retry='y'
a=$attempts
while [[ $retry = 'y' ]]; do

    generate_number 1 $limit
    Y=$?

    while [[ $X != $Y && $a > 0 ]]; do
        echo 'Attempts left:' $a
        read -p 'Please enter your guess 1-'$limit' ' X
        compare_numbers $X $Y
        let a=$a-1
    done
    if [[ $X != $Y ]];
    then
        echo Shame on you!
    else
        echo Good job!
        read -p 'Retry(y - yes)? ' retry
        X=$limit+1
        let a=$attempts
    fi
done
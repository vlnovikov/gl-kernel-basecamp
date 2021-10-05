#!/bin/bash

l=`find $1 -mindepth 1 -maxdepth 1 -mtime +30 -printf '%f\n'`
for f in $l; do
    mv $1/$f $1/~$f
done
#!/bin/sh

touch -- "_123.txt" "~234.txt" "-456.txt" "ooo.txt" "ooo.tmp" "mytmp"
mkdir -p hello/test
cd hello/test/
touch -- "_123.txt" "~234.txt" "-456.txt" "ooo.txt" "ooo.tmp" "mytmp"

#!/bin/bash

echo 'HHHellloow worlddd   !!!11111' > input.dat
./encoder input.dat encoded.dat
./decoder encoded.dat decoded.dat
diff -q input.dat decoded.dat
rm input.dat encoded.dat decoded.dat
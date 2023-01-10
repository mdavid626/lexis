#!/bin/sh

i=$1

cat ./tests/good_src/test$i.out
echo
echo "********************************************************************************"
cat ./tests/good_src/test$i-result.out
echo






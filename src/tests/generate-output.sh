#!/bin/sh

for i in {1..24}
do
  fbc ./freebasic_src/test$i.bas
  ./freebasic_src/test$i < good_src/test$i.in > good_src/test$i.out
  rm ./freebasic_src/test$i
done
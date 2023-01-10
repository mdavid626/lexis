#!/bin/sh

project="ifj-project"


echo
echo "********************************************************************************"
echo "* Compiling project                                                            *"
echo "********************************************************************************"
echo 
gmake EXECUTABLE=$project

echo
echo "********************************************************************************"
echo "* Running test - wrong source                                               *"
echo "********************************************************************************"
echo

i=$1
echo "* Test Wrong $i: ${message[$i-1]}"
rm valgrind.txt
valgrind --tool=memcheck --leak-check=yes --show-reachable=yes --log-file=valgrind.txt ./$project ./tests/wrong_src/wrong$i.src  
cat valgrind.txt
echo "--------------------------------------------------------------------------------"
echo 

gmake clean EXECUTABLE=$project






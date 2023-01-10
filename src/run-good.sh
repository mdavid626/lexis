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
echo "* Running test - checking output                                               *"
echo "********************************************************************************"
echo

i=$1
echo "* Test $i"
rm valgrind.txt
valgrind --tool=memcheck --leak-check=yes --show-reachable=yes --log-file=valgrind.txt ./$project ./tests/good_src/test$i.src < ./tests/good_src/test$i.in > ./tests/good_src/test$i-result.out
diff ./tests/good_src/test$i.out ./tests/good_src/test$i-result.out >./tests/good_src/test$i.diff
if [ -s ./tests/good_src/test$i.diff ]
  then  echo "  !!failed!!"
else 
  echo "  **success**"
fi
cat valgrind.txt
  
echo "--------------------------------------------------------------------------------"
echo 

gmake clean EXECUTABLE=$project






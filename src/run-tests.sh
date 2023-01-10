#!/bin/bash

project="ifj-project"

verbose=0
if [ "$1" = "-v" -o "$2" = "-v" -o "$3" = "-v" -o "$4" = "-v" ]
  then
  verbose=1
fi

nodel=0
if [ "$1" = "-n" -o "$2" = "-n" -o "$3" = "-n" -o "$4" = "-n" ]
  then
  nodel=1
fi

echo
echo "********************************************************************************"
echo "* Compiling project                                                            *"
echo "********************************************************************************"
echo 
gmake EXECUTABLE=$project

echo
echo "********************************************************************************"
echo "* Running tests - wrong sources                                                *"
echo "********************************************************************************"
echo

message=( 
'Dim -> Dm' 
'Else -> Ele'
'Return -> Reurn'
'2x ta ista promenna'
'2x ta ista promenna, ale iny typ'
'Nedeklarovane promenne'
'Nedeklarovane funkce'
'Nedefinovane funkce'
'Nekompatibilita typu'
'Nekompatibilita typu pri volani funkce'
'Nekompatibilita typu pri volani funkce 2'
'Nekompatibilita typu pri volani funkce 3'
'Nevhodny pocet parametru pri volani funkce'
'Identifikator je klicove slovo'
'Identifikator zacina na cislo'
'Jmeno funkcie zacina na cislo'
'Deleni nulou'
'Nevhodne typy'
'Chybne konstanty'
'Identifikator zacina na @'
'Chybna celociselna konstanta'
'Chybna desatinna konstanta'
'Chybna Retezcova konstanta'
'Neexistujuci datovy typ'
'Blokove komentare'
'Chybny napsany Input'
'Chybny napsany Input 2'
'Chybne napsany Print'
'Chybne napsany Print 2'
'Chybny prikaz prirazeni'
'Chybny IF'
'Chybny IF 2'
'Chybny DO WHILE'
'Chybne vyrazy 1'
'Chybne vyrazy 2'
'Chybne vyrazy 3'
'Chybne vyrazy 4'
'Chybne vyrazy 5'
'Chybne vyrazy 6'
'Chybne vyrazy 7'
'Vestavene funkce 1 - deklarovana uzivatelem'
'Vestavene funkce 2'
'Deklaracia funkcii je po definicii'
'Funkcia 2x deklarovana'
'Funkcia 2x definovana'
'Funkcia a parameter maje stejne jmeno'
'Funkcia vs promenne'
'Funkcia deklaracia - definicia ine parametre'
'Ve funkcii este raz definovana parameter'
'Return in scope'
'Same params'
'Compare mismatch'
)

for i in {1..52}
do
  echo "* Test Wrong $i: ${message[$i-1]}"
  rm valgrind.txt
  valgrind --log-file=valgrind.txt ./$project ./tests/wrong_src/wrong$i.src  
  cat valgrind.txt | sed 's/^.*[=]/*/g' | grep bytes 
  if [ $verbose = 1 ]
    then
    echo "Do you want to view the source file (Y/N)?"
    read z
    if [ "$z" = "Y" -o "$z" = "y" ]
      then
      cat ./tests/wrong_src/wrong$i.src
    fi
  fi
  echo "--------------------------------------------------------------------------------"
  echo 
done                                                                      

echo "********************************************************************************"
echo "* Running tests - checking output                                              *"
echo "********************************************************************************"
echo

succ_tests=0
total_tests=0
for i in {1..24}
do
  echo "* Test $i"
  rm valgrind.txt
  valgrind --log-file=valgrind.txt ./$project ./tests/good_src/test$i.src < ./tests/good_src/test$i.in > ./tests/good_src/test$i-result.out
  diff ./tests/good_src/test$i.out ./tests/good_src/test$i-result.out >./tests/good_src/test$i.diff
  if [ -s ./tests/good_src/test$i.diff ]
    then  echo "  !!failed!!"
  else 
    echo "  **success**"
    succ_tests=`expr $succ_tests + 1`
  fi
  cat valgrind.txt | sed 's/^.*[=]/*/g' | grep bytes
  
  if [ $verbose = 1 ]
    then
    echo "Do you want to view the source file (Y/N)?"
    read z
    if [ "$z" = "Y" -o "$z" = "y" ]
      then
      cat ./tests/good_src/test$i.src
    fi
  fi
  
  echo "--------------------------------------------------------------------------------"
  echo 
  
  total_tests=`expr $total_tests + 1`
done

echo "Successed tests: $succ_tests"
echo "Total Number of Tests: $total_tests"
echo

gmake clean EXECUTABLE=$project
rm valgrind.txt

if [ $nodel -eq 0 ]
  then
    echo "deleting temp in tests/good_src..."
    rm ./tests/good_src/*result*
    rm ./tests/good_src/*.diff
fi








/*****************************************************
** Soubor:    ial-test.c
** Encoding:  ISO-8859-2
** Datum:     2010/29/11
** Projekt:   IAL, IFJ
** Nazev:     Implementace interpretu imperativniho jazyka IFJ10
** Popis:     Test vestavenej funkcii.
*****************************************************
**/

#include <stdio.h>

#include "str.h"
#include "ial.h"

int main()
{
    string str;
    strInit(&str);
    strAddChar(&str, 'a');
    strAddChar(&str, 'b');
    strAddChar(&str, 'c');
    strAddChar(&str, 'd');
    strAddChar(&str, 'e'); 
    
    string substr;
    strInit(&substr);
    strAddChar(&substr, 'c');
    strAddChar(&substr, 'b');
    strAddChar(&substr, 'c');
    strAddChar(&substr, 'd');
    strAddChar(&substr, 'e');
    
    int result = findStr(str, substr);
    
    printf("str: %s\n", str.str);
    printf("substr: %s\n", substr.str);
    printf("result: %d\n", result);
    
    strFree(&str);
    strFree(&substr);
    
    
    string puvodni;
    strInit(&puvodni);
    strAddChar(&puvodni, 'b');
    strAddChar(&puvodni, 'c');
    strAddChar(&puvodni, 'e');
    strAddChar(&puvodni, 'a');
    strAddChar(&puvodni, 'd');
    
    string razeni = sortStr(puvodni);
    
    printf("puvodni str: \"%s\"\n", puvodni.str);
    printf("razeni str: \"%s\"\n", razeni.str);
    
    strFree(&puvodni);
    strFree(&razeni);
    
    return 0;
}

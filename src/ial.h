
/*****************************************************
** Soubor:    ial.h
** Encoding:  ISO-8859-2
** Datum:     2010/29/11
** Autor:     David Molnár, xmolna02
** Projekt:   IAL, IFJ
** Nazev:     Implementace interpretu imperativniho jazyka IFJ10
** Popis:     Hlavicka vestavenej funkcii.
*****************************************************
**/

#include "str.h"

#ifndef IAL_H
#define IAL_H

#define ASCII_COUNT 256

#define max(a,b)	(((a) > (b)) ? (a) : (b))
#define min(a,b)	(((a) < (b)) ? (a) : (b))

/* deklarace vestavenej funkcii */
int findStr(string str, string substr);
string sortStr(string str); 

#endif // IAL_H

// konec ial.h

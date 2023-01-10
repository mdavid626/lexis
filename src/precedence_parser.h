
/*****************************************************
** Soubor:    precedence_parser.h
** Encoding:  ISO-8859-2
** Datum:     2010/14/11
** Autor:     Tomáš Pop, xpopto01
** Projekt:   IAL, IFJ
** Nazev:     Implementace interpretu imperativniho jazyka IFJ10
** Popis:     Hlavicka Precedencniho parseru.
*****************************************************
**/

// Funkce pro syntaktickou analyzu zdola nahoru
//
// @param  result ukazatel na strukturu tData zde se ulozi vysledek vyrazu
// @return 0 - OK 
//         1 - ERROR lexikalni chyba
//         2 - ERROR syntakticka chyba
//         3 - ERROR semanticka chyba
//         5 - ERROR interni chyba
#ifndef PRECEDENCE_PARSER_H
#define PRECEDENCE_PARSER_H
#include "globals.h"

int expresionParser(tData *result);

#endif

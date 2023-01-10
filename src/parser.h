/*****************************************************
** Soubor:    parser.h
** Encoding:  ISO-8859-2
** Datum:     2010/14/11
** Autor:     Tomá¹ Mészáros, xmesza03
** Projekt:   IAL, IFJ
** Nazev:     Implementace interpretu imperativniho jazyka IFJ10
** Popis:     Hlavicka Syntaktickeho analyzatoru.
*****************************************************
**/

#ifndef PARSER_H_DEFINED
#define PARSER_H_DEFINED

#include <stdlib.h>
#include <stdio.h>
#include "scaner.h"

extern TLex token;

/**
 * @brief Volání parseru
 * @return Funkce vrati chybove kody definovane ve vyctu ERROR_NUMBERS (globals.h)
 */
int parse(void);

/**
 * @brief Vygeneruje jedinecny identifikator
 * @param s string do ktereho bude vlozen identifikator
 */
void newKey(string s);

#endif

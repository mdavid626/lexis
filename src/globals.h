/***************************************************
** PROJEKT do predmetu IFJ a IAL
** Encoding:  ISO-8859-2
** Datum:     2010/30/11
** Autor:     Tom� M�sz�ros, xmesza03
** Nazev:     Implementace interpretu imperativniho jazyka IFJ10
** varianta:  Tym 6, varianta b/2/I
*****************************************************
**/

#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdio.h>
#include <stdlib.h>

#include "table.h"
#include "i_list.h"

//#define SEM_OFF
//#define EXP_PARSER_OFF
//#define INSTRUCTIONS_OFF

#ifdef SEM_OFF
    #define INSTRUCTIONS_OFF
#endif

/* neni implementovan, mnel by vypsat jak vypada instrukcni paska, nebo kazda
 * instrukce ohlasi co chce provest (asi lepsi reseni) */
//#define PRINT_INSTRUCTIONS

#define NDEBUG


/*
 * CHYBOV� K�DY
 */
enum ERROR_NUMBERS
{
    OK = 0,             // cinnost interpretu probehla bez chyb
    LEX_ERROR,          // chyba v programu v ramci lexikalni analyzy
    SYNTAX_ERROR,       // chyba v programu v ramci syntakticke analyzy
    SEM_ERROR,          // chyba v programu v ramci semantickych kontrol
    RUNTIME_ERROR,      // chyba za behu programu
    INTERNAL_ERROR,     // interni chyba interpretu
};


/*
 * Glob�ln� seznam tabulek symbol� (pot�ebn� pro realizace rekurze)
 * Last je GST
 */
extern tList STableList;

/*
 * Filehandle pro zdrojov� soubor
 */
extern FILE *srcFile;

/*
 * Seznam instrukc� pro interpret
 */
extern tInstructionList IList;



/* ********************** DEKLARACE FUNKC� ********************************** */
/*                                                                            */

/**
 * @brief Vyp�e chybov� hl�en� na stderr
 * @param errnum K�d chyby
 */
void printError(enum ERROR_NUMBERS errnum);

/**
 * @brief Inicializuje v�echny pot�ebn� zdroje ( vtvo�� seznam tabulek symbol�,
 * instruk�n� p�sku, etc..)
 */
void initAllResources(void);

/**
 * @brief Uvoln� v�echny zdroje
 */
void freeAllResources(void);

#endif //GLOBALS_H

/***************************************************
** PROJEKT do predmetu IFJ a IAL
** Encoding:  ISO-8859-2
** Datum:     2010/30/11
** Autor:     Tomá¹ Mészáros, xmesza03
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
 * CHYBOVÉ KÓDY
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
 * Globální seznam tabulek symbolù (potøební pro realizace rekurze)
 * Last je GST
 */
extern tList STableList;

/*
 * Filehandle pro zdrojový soubor
 */
extern FILE *srcFile;

/*
 * Seznam instrukcí pro interpret
 */
extern tInstructionList IList;



/* ********************** DEKLARACE FUNKCÍ ********************************** */
/*                                                                            */

/**
 * @brief Vypí¹e chybové hlá¹ení na stderr
 * @param errnum Kód chyby
 */
void printError(enum ERROR_NUMBERS errnum);

/**
 * @brief Inicializuje v¹echny potøebné zdroje ( vtvoøí seznam tabulek symbolù,
 * instrukèní pásku, etc..)
 */
void initAllResources(void);

/**
 * @brief Uvolní v¹echny zdroje
 */
void freeAllResources(void);

#endif //GLOBALS_H

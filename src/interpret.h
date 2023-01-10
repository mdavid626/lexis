
/*****************************************************
** Soubor:    interpret.h
** Encoding:  ISO-8859-2
** Datum:     2010/30/11
** Autor:     Lukáš Langer, xlange02
** Projekt:   IAL, IFJ
** Nazev:     Implementace interpretu imperativniho jazyka IFJ10
** Popis:     Implementace interpretu vcetne generovani instrukcii
**            a jejich interpretaci
*****************************************************
**/



#ifndef INTERPRET_H
#define INTERPRET_H

#include "i_list.h"

#define MAX_STACK 500




//extern int ERROR_CODE;

/* jednotlive typy instrukcii
   vsechny instrukce zacinaji prefixem "I_"
   pokud instrukce provadi vysledek je ulozen v op3
   pokud instrukce potrebuje operandy museji byt ulozeny v op1, pripadne op2
   Popis operandu instrukcii:
    I_PLUS              op3 = op1 + op2;
    I_MINUS             op3 = op1 - op2;
    I_MULT              op3 = op1 * op2;
    I_DIV               op3 = op1 / op2;
    I_NOT               op3 = !op1;
    I_GOTO              goto op3;
    I_IF_GOTO           if (op1) goto op3;
    I_CALL              call op3;
    I_RETURN            return;
    I_LABEL             label op1;
    I_MOVE              op3 = op1;
    I_INT_TO_DOUBLE     op3 = (double) op1;
    I_READ
    I_WRITE             printf("% d", op1);
    I_ZERO              op1 = 0;
    I_TEST_ZERO         if (op1 == 0) op3 = 1; else op3 = 0;
    I_TEST_POSITIVE     if (op1 > 0) op3 = 1; else op3 = 0;
    I_FIND              op3 = find(op1, op2);
    I_SORT              op3 = sort(op1);

    I_STOP              return EXIT_SUCCESS;
*/

// samotna interpretace jiz vygenerovaneho seznamu instrukcii
int interpret(void);

// funkce na generovani instrukce a vlozeni ho do seznamu instrukcii
void generate(int type, void *op1, void *op2, void *op3);

// nastaveni instrukcniho seznamu
void setInstructionList(tInstructionList *List);

#endif // INTERPRET_H

/* konec interpret.h */

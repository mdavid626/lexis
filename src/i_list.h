
/*****************************************************
** Soubor:    i_list.h
** Encoding:  ISO-8859-2
** Datum:     2010/30/11
** Autor:     Lukáš Langer, xlange02
** Projekt:   IAL, IFJ
** Nazev:     Implementace interpretu imperativniho jazyka IFJ10
** Popis:     Hlavicka seznamu instrukcniho listu a zasobniku.
** Poznamky:  Seznam vygenerovanych instrukcii pro interpret
**            Jednotlivou instrukcii vklada do seznamu intrukcii.
*****************************************************
**/

//#include "interpret.h"

#ifndef I_LIST_H
#define I_LIST_H


enum INSTRUCTION_SET
{
    // instrukce pro allokaci a deallokaci
    I_ALLOC = 0,

    // instrukce pro vlozeni a vybrani ze interpretovaciho zasobniku
    I_PUSH,

    // aritmeticke instrukce
    I_PLUS,
    I_MINUS,
    I_MULT,
    I_DIV,

    // instrukce negace
    I_NOT,

    // instrukce nepodmineneho skoku
    I_GOTO,
    // instrukce podmineneho skoku
    I_IF_GOTO,

    // instrukce volani funkce
    I_CALL,
    // instrukce navratu z funkce
    I_RETURN,

    // navesti
    I_LABEL,
    // navesti pro funkci
    I_FUNCTION,

    // vstupni a vystupni instrukce
    I_READ,
    I_WRITE,

    // instrukce konverzni
    I_INT_TO_DOUBLE,

    // instrukce presunu
    I_MOVE,

    // instrukce nahrani konstanty do promene
    I_CONST,

    // instrukce vynulovani
    I_ZERO,

    // instrukce testovaci
    I_TEST_G,
    I_TEST_GEQ,
    I_TEST_L,
    I_TEST_LEQ,
    I_TEST_EQ,
    I_TEST_NEQ,

    // vestavane funkce pro vyhledani a serazeni retezce
    I_FIND,
    I_SORT,

    // instrukce konce programu
    I_STOP,
};


/* deklarace struktury obsahujici instrukci 3addresneho kodu */
typedef struct
{
    enum INSTRUCTION_SET type;
    void *op1;
    void *op2;
    void *op3;
} tTriCode;


/* deklarace polozky instrukcniho listu */
typedef struct list_item
{
    tTriCode Instruction;
    struct list_item *Next;
} tListItem;


/* deklarace instrukcniho seznamu */
typedef struct
{
    struct list_item *First;
    struct list_item *Last;
    struct list_item *Active;
} tInstructionList;


/* prototypy procedur pro seznam */

void ListInit(tInstructionList *List);
void ListFree(tInstructionList *List);
int ListInsertLast(tInstructionList *List, tTriCode Instruction);
void ListFirst(tInstructionList *List);
void ListNext(tInstructionList *List);
void ListGoto(tInstructionList *List, void *ptr);
void *ListGetLast(tInstructionList *List);
tTriCode GetInstruction(tInstructionList *List);


#endif // I_LIST_H

/* konec i_list.h */

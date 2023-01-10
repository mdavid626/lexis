
/*****************************************************
** Soubor:    scaner.h
** Encoding:  ISO-8859-2
** Datum:     2010/14/11
** Autor:     Lukáš Langer, xlange02
** Projekt:   IAL, IFJ
** Nazev:     Implementace interpretu imperativniho jazyka IFJ10
** Popis:     Hlavicka Lexikalniho analyzatoru.
*****************************************************
**/

#ifndef SCANER_H
#define SCANER_H

#include "str.h"

#define APOSTROPHE 39


// pocet klicovych slov
#define KEY_NUM 20

extern unsigned int line_num;

// deklarace pole klicovych slov
extern const char *KEY_WORDS[];

/* deklarace typu tokenu */
enum TOKEN_TYPE
{
    // klicove slova

    AS = 0,
    DECLARE,
    DIM,
    DO,
    DOUBLE,
    ELSE,
    END,
    FIND,
    FUNCTION,
    IF,
    INPUT,
    INTEGER,
    LOOP,
    PRINT,
    RETURN,
    SCOPE,
    SORT,
    STRING,
    THEN,
    WHILE,

    ID,             // identifikator

    CONST_INT,      // celociselna konstanta int
    CONST_DOUBLE,   // desetinna konstanta double
    CONST_STRING,   // retezcova konstanta typu string

    PLUS,           // aritmeticky operator plus
    MINUS,          // aritmeticky operator minus
    MULT,           // aritmeticky operator krat
    DIV_INT,        // aritmeticky operator celociselneho deleni
    DIV_DOUBLE,     // aritmeticky operator deleni double

    EQ,             // priradovaci nebo porovnavaci operator rovna se
    NOT_EQ,         // porovnavaci operator nerovna se
    LEFT_LESS,      // porovnavaci operator levy < pravy
    LEFT_LESS_EQ,   // porovnavaci operator levy <= pravy
    RIGHT_LESS,     // porovnavaci operator levy > pravy
    RIGHT_LESS_EQ,  // porovnavaci operator levy >= pravy
 
    LEFT_BRACKET,   // leva zavorka
    RIGHT_BRACKET,  // prava zavorka

    COMMA,          // carka pro odeleni seznamu identifikatoru
    END_OF_LINE,    // token reprezentujici konec radku

    SEMICOLON,      // strednik pro prikazy Input a Print

    END_OF_FILE,    // token reprezentujici konec zdrojoveho programu


};

/* deklarace struktury tokenu */
typedef struct
{
    enum TOKEN_TYPE type;
    string attribut;
} TLex;


/* deklarace funkce realizujici lexikalni analyzator */
int getNextToken(FILE *f, TLex *token);

/* inicializace tokenu */
TLex tokenInit(void);

#endif // SCANER_H

// konec scaner.h

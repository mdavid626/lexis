
/*****************************************************
** Soubor:    scaner.c
** Encoding:  ISO-8859-2
** Datum:     2010/14/11
** Autor:     Lukáš Langer, xlange02
** Projekt:   IAL, IFJ
** Nazev:     Implementace interpretu imperativniho jazyka IFJ10
** Popis:     zdrojovy kod Lexikalniho analyzatoru.
*****************************************************
**/

#include <stdio.h>
#include <ctype.h>

#include "scaner.h"
#include "globals.h"

unsigned int line_num = 1;

// pole klicovych slov
// index do pole je zaroven typ tokenu klicoveho slova
const char *KEY_WORDS[] =
{
    "AS",
    "DECLARE",
    "DIM",
    "DO",
    "DOUBLE",
    "ELSE",
    "END",
    "FIND",
    "FUNCTION",
    "IF",
    "INPUT",
    "INTEGER",
    "LOOP",
    "PRINT",
    "RETURN",
    "SCOPE",
    "SORT",
    "STRING",
    "THEN",
    "WHILE",
};

TLex tokenInit(void)
{
    TLex token;
    token.attribut = newString();
    token.type = -1;
    return token;
}


/* Hlavni funkce lexikalniho analyzatoru */
int getNextToken(FILE *f, TLex *token)
{
    int c;                  // cteny znak

    int state = 0;          // pocatecni stav

    strClear(token->attribut);  // vymazani attributu

    while(1)
    {
        c = getc(f);

        switch (state)
        {

            // pocatecni stav s
            case 0:
            if (c == ' ' || c == '\t' || c == '\r')
            { // preskoceni bylich znaku
                state = 0;
            }
            else if (c == APOSTROPHE)
            { // radkovy komentar
                state = 1;
            }
            else if (c == '/')
            { // stav pro deleni double nebo zacatku blokoveho komentare
              // v stavu 2 se kontroluje zda se jedna o blokovy komentar
              // alebo deleni double
                state = 2;
            }
            else if (c == '_' || isalpha(c))
            {
                strAddChar(token->attribut, toupper(c));
                state = 5;
            }
            else if (c == '+')
            {
                token->type = PLUS;
                return OK;
            }
            else if (c == '-')
            {
                token->type = MINUS;
                return OK;
            }
            else if (c == '*')
            {
                token->type = MULT;
                return OK;
            }
            else if (c == '\\')
            {
                token->type = DIV_INT;
                return OK;
            }
            else if (c == '=')
            {
                token->type = EQ;
                return OK;
            }
            else if (c == '<')
            { // prechod do stavu kontroly porovnavani
                state = 6;
            }
            else if (c == '>')
            { // prechod do stavu kontroly porovnavani
                state = 7;
            }
            else if (c == ',')
            { // carka pro odeleni identifikatoru
                token->type = COMMA;
                return OK;
            }
            else if (c == ';')
            { // strednik
                token->type = SEMICOLON;
                return OK;
            }
            else if (c == '(')
            {
                token->type = LEFT_BRACKET;
                return OK;
            }
            else if (c == ')')
            {
                token->type = RIGHT_BRACKET;
                return OK;
            }
            else if (c == '"')
            { // zacatek retezce
                state = 8;
            }
            else if (isdigit(c))
            { // zacatek cislove konstanty
                state = 9;
                strAddChar(token->attribut, c);
            }
            else if (c == '\n')
            {
                line_num++;
                state = 18;
            }
            else if (c == EOF)
            {
                token->type = END_OF_FILE;
                return OK;
            }
            else
                return LEX_ERROR;
            break;


            // stav pro radkovy komentar
            case 1:
            if (c == '\n')
            { // konec komentare signalizuje konec radku
                line_num++;
                state = 18;
            }
            break;


            // stav kontrolujici zacatek blokoveho komentare nebo double deleni
            case 2:
            if (c == APOSTROPHE)
            { // sekvence znaku pro blokovy komentar
                state = 3;
            }
            else
            { // nejedna se o blokovy komentar tudiz se jedna o deleni double
                token->type = DIV_DOUBLE;    // nastaveni typu tokenu
                ungetc(c, f);                // vraceni cteciho znaku
                return OK;               // indikace platneho tokenu
            }
            break;


            // stav pro blokovy komentar
            case 3:
            if( c == '\n' ) line_num++;
            if (c == APOSTROPHE)
            { // dokud nenastane posloupnost '\ vse se ignoruje
                state = 4;
            }
            else if (c == EOF)
            { // nastal konec souboru nez skoncil blokovy komentar
                return LEX_ERROR;
            }
            break;


            // stav kontrolujici ukonceni blokoveho komentare
            case 4:
            if (c == '/')
            { // korektni ukonceni blokoveho komentare
                state = 0;
            }
            else
                return LEX_ERROR; // za apostrofem nenasledovalo lomitko, tudiz lexikalni chyba
            break;


            // stav pro identifikatory nebo klicove slova
            case 5:
            if (isalnum(c) || c == '_')
            {
                strAddChar(token->attribut, toupper(c));
            }
            else
            { // identifikator nebo klicove slovo
                ungetc(c, f);      // vraceni nacteneho znaku
                for (int i = 0; i < KEY_NUM; i++)
                { // prochadzeni tabulky klicovych slov
                    if (strCmpConstStr(token->attribut, KEY_WORDS[i]) == 0)
                    { // naslo se klicove slovo v tabulce
                        token->type = i;
                        return OK;
                    }
                }
                // nejedna se o klicove slovo tudiz je to identifikator
                token->type = ID;
                return OK;
            }
            break;


            // stav pro kontrolu porovnavani <
            case 6:
            if (c == '>')
            { // operator <>
                token->type = NOT_EQ;
                return OK;
            }
            else if (c == '=')
            { // operator <=
                token->type = LEFT_LESS_EQ;
                return OK;
            }
            else
            { // opetaor <
                ungetc(c, f);   // vracni nacteneho znaku
                token->type = LEFT_LESS;
                return OK;
            }
            break;


            // stav pro kontorlu porovnavani >
            case 7:
            if (c == '=')
            { // operator >=
                token->type = RIGHT_LESS_EQ;
                return OK;
            }
            else
            { // operator >
                ungetc(c, f);
                token->type = RIGHT_LESS;
                return OK;
            }
            break;


            // stav pro kontrolu a skladani retezce
            case 8:
            if(c == EOF || c == '\n') return LEX_ERROR;
            if(c == '\\') state = 22;
            else if (c == '"')
            { // retezec

                token->type = CONST_STRING;
                return OK;
            }
            else
            {
                strAddChar(token->attribut, c);
            }
            break;


            // || c == 't' || c == 'n' || c == '\\'

            case 22:
                if( c == '"' || c == '\\')
                    strAddChar(token->attribut, c);
                else if (c == 't')
                    strAddChar(token->attribut, '\t');
                else if (c == 'n')
                    strAddChar(token->attribut, '\n');
                else
                    return LEX_ERROR;
                state = 8;
            break;


            // stav pro kontrolu cislenych konstant
            case 9:
            if (isdigit(c))
            {
                state = 9;
                strAddChar(token->attribut, c);
            }
            else if (c == '.')
            { // zacatek desetine casti double
                state = 10;
                strAddChar(token->attribut, c);
            }
            else if (c == 'e' || c == 'E')
            { // zacatek exponencialni casti
                state = 11;
                strAddChar(token->attribut, c);
            }
            else if (c == '+' || c == '-')
            {
                state = 12;
                strAddChar(token->attribut, c);
            }
            else
            { // celociselna konstanta
                ungetc(c, f);
                token->type = CONST_INT;
                return OK;
            }
            break;


            // stav pro kontrolu desetine tecky
            case 10:
            if (isdigit(c))
            {
                state = 13;
                strAddChar(token->attribut, c);
            }
            else
            { // za desetinou teckou nenasleduje cislo, tudiz lexikalni chyba
                return LEX_ERROR;
            }
            break;


            // stav pro kontrolu exponencialne casti double
            case 11:
            if (isdigit(c))
            {
                state = 16;
                strAddChar(token->attribut, c);
            }
            else
                return LEX_ERROR;
            break;

            // stav pro kontrolu + a -
            case 12:
            if (c == 'e' || c == 'E')
            {
                state = 11;
                strAddChar(token->attribut, c);
            }
            else
                return LEX_ERROR;
            break;


            // stav pro pridavani cislic
            case 13:
            if (isdigit(c))
            {
                state = 13;
                strAddChar(token->attribut, c);
            }
            else if (c == 'e' || c == 'E')
            {
                state = 11;
                strAddChar(token->attribut, c);
            }
            else if (c == '+' || c == '-')
            {
                state = 12;
                strAddChar(token->attribut, c);
            }
            else
            {
                ungetc(c, f);
                token->type = CONST_DOUBLE;
                return OK;
            }
            break;

            /*
            // stav pro kontrolu exponencialne casti cisla s des. teckou
            case 14:
            if (c == '+' || c == '-')
            {
                state = 15;
                strAddChar(token->attribut, c);
            }
            else if (isdigit(c))
            {
                state = 16;
                strAddChar(token->attribut, c);
            }
            else
                return LEX_ERROR;
            break;


            // stav pro kontrolu + a - u exponentu
            case 15:
            if (isdigit(c))
            {
                state = 16;
            }
            else
                return LEX_ERROR;
            break;
            */


            // ukoncujici stav pro cislo double
            case 16:
            if (isdigit(c))
            {
                strAddChar(token->attribut, c);
                state = 16;
            }
            else
            {
                ungetc(c, f);
                token->type = CONST_DOUBLE;
                return OK;
            }
            break;

            // kdyz vic EOL pride po sobe, tak posledny se vraci jako END_OF_LINE
            // ostatne prekroci
            case 18:
                if (c == '\n') line_num++;
                if( c == APOSTROPHE ) state = 1;
                else if( c == '/' )
                {
                    state = 19;
                }
                else if( c != '\n' && c != ' ' && c != '\t' && c != '\r')
                {
                    ungetc(c, f);
                    token->type = END_OF_LINE;
                    return OK;
                }
            break;

            case 19:
                if (c == APOSTROPHE)
                { // sekvence znaku pro blokovy komentar
                    state = 20;
                }
                else
                { // nejedna se o blokovy komentar tudiz se jedna o deleni double
                    token->type = DIV_DOUBLE;    // nastaveni typu tokenu
                    ungetc(c, f);                // vraceni cteciho znaku
                    return OK;               // indikace platneho tokenu
                }
            break;

            case 20:
                if( c == '\n' ) line_num++;
                if (c == APOSTROPHE)
                { // dokud nenastane posloupnost '\ vse se ignoruje
                    state = 21;
                }
                else if (c == EOF)
                { // nastal konec souboru nez skoncil blokovy komentar
                    return LEX_ERROR;
                }
            break;

            case 21:
                if (c == '/')
                { // korektni ukonceni blokoveho komentare
                    state = 18;
                }
                else
                    return LEX_ERROR; // za apostrofem nenasledovalo lomitko, tudiz lexikalni chyba
            break;
        }
    }
}

/* konec scaner.c */

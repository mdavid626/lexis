
/*****************************************************
** Soubor:    str.h
** Encoding:  ISO-8859-2
** Datum:     2010/14/11
** Autor:     Tomáš Mészáros, xmesza03
** Projekt:   IAL, IFJ
** Nazev:     Implementace interpretu imperativniho jazyka IFJ10
** Popis:     Hlavicka modulu pro praci s retezcema.
** Poznamka:  Pouzito ze vzoroveho projektu
*****************************************************
**/

#ifndef STR_H
#define STR_H

typedef struct t_string
{
  char* str;		// misto pro dany retezec ukonceny znakem '\0'
  int length;		// skutecna delka retezce
  int allocSize;	// velikost alokovane pameti
}* string;

typedef struct t_str_record
{
    string str;
    struct t_str_record* next;
}TStrRecord;

typedef struct t_collector
{
    TStrRecord* first;
    TStrRecord* last;
}TStrCollector;

extern TStrCollector string_collector;

/**
 * @brief Inicializuje kollektor stringov, co sluzi na hromadne dealokovanie
 * vsetkych alokovanich stringov (volat v initAllresources)
 */
void initStrCollector(void);

/**
 * @brief Dealokuje vsetky premenne typu string v programu (volat na konci main
 * alebo zrovna v tom freeAllresources
 */
void deleteStrCollector(void);

/**
 * @brief Vrati nachystany naalokovany string
 * @return
 */
string newString(void);

//int strInit(string s);
void strFree(string s);

void strClear(string s);
string strWrite(const char*);
int strAddChar(string s1, char c);
int strCopyString(string s1, string s2);
int strAddString(string s1, string s2, string s3);
int strCmpString(string s1, string s2);
int strCmpConstStr(string s1, const char* s2);

void strReverse(string s);

char *strGetStr(string s);
int strGetLength(string s);

// doplnene vlastni funkce

int strtoint(string s);
double strtodouble(string s);

#endif // STR_H

/* konec str.h */

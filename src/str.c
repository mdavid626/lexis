
/*****************************************************
** Soubor:    str.c
** Encoding:  ISO-8859-2
** Datum:     2010/14/11
** Autor:     Tomáš Mészáros, xmesza03
** Projekt:   IAL, IFJ
** Nazev:     Implementace interpretu imperativniho jazyka IFJ10
** Popis:     Kod modulu pro praci s retezcema.
** Poznamka:  Pouzito ze vzoroveho projektu
*****************************************************
**/

//jednoducha knihovna pro praci s nekonecne dlouhymi retezci
#include <malloc.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "str.h"

#define STR_LEN_INC 8
// konstanta STR_LEN_INC udava, na kolik bytu provedeme pocatecni alokaci pameti
// pokud nacitame retezec znak po znaku, pamet se postupne bude alkokovat na
// nasobky tohoto cisla

#define STR_ERROR   1
#define STR_SUCCESS 0

TStrCollector string_collector;



void initStrCollector(void)
{
    string_collector.first = NULL;
    string_collector.last = NULL;
}

int strInit(string s)
// funkce vytvori novy retezec
{
   if ((s->str = (char*) malloc(STR_LEN_INC)) == NULL)
   {
       s->str = NULL;
       s->length = 0;
       s->allocSize = 0;
       return STR_ERROR;
   }

   s->str[0] = '\0';
   s->length = 0;
   s->allocSize = STR_LEN_INC;

   TStrRecord* record = (TStrRecord*) malloc( sizeof(TStrRecord) );
   if( record == NULL)
   {
       free(s->str);
       s->str = NULL;
       s->allocSize = 0;
       return STR_ERROR;
   }
   record->str = s;
   record->next = NULL;

   if( string_collector.first == NULL )
   {
       string_collector.first = record;
       string_collector.last = record;
   }
   else
   {
       string_collector.last->next = record;
       string_collector.last = string_collector.last->next;
   }

   return STR_SUCCESS;
}

string newString(void)
{
    string s = (string) malloc(sizeof(struct t_string) );
	if( s != NULL)
	{
		if( strInit(s) == STR_ERROR )
		{
			free(s);
			s = NULL;
		}
	}
    return s;
}

void deleteStrCollector(void)
{
    TStrRecord* temp = string_collector.first;
    while( string_collector.first != NULL )
    {

        if( temp->str != NULL )
        {
			free( temp->str->str );
			free(temp->str);
        }

        string_collector.first = temp->next;
        free(temp);
		temp = string_collector.first;
    }
}

void strFree(string s)
// funkce uvolni retezec z pameti
{
   free(s->str);
   s->str = NULL;
   s->allocSize = 0;
   s->length = -1;
}

void strClear(string s)
// funkce vymaze obsah retezce
{
   s->str[0] = '\0';
   s->length = 0;
}

int strAddChar(string s1, char c)
// prida na konec retezce jeden znak
{
   if (s1->length + 1 >= s1->allocSize)
   {
      // pamet nestaci, je potreba provest realokaci
      if ((s1->str = (char*) realloc(s1->str, s1->length + STR_LEN_INC)) == NULL)
         return STR_ERROR;
      s1->allocSize = s1->length + STR_LEN_INC;
   }
   s1->str[s1->length] = c;
   s1->length++;
   s1->str[s1->length] = '\0';
   return STR_SUCCESS;
}

int strCopyString(string s1, string s2)
// prekopiruje retezec s2 do s1
{
   int newLength = s2->length;
   if (newLength >= s1->allocSize)
   {
      // pamet nestaci, je potreba provest realokaci
      if ((s1->str = (char*) realloc(s1->str, newLength + 1)) == NULL)
         return STR_ERROR;
      s1->allocSize = newLength + 1;
   }
   strcpy(s1->str, s2->str);
   s1->length = newLength;
   return STR_SUCCESS;
}

int strAddString(string s1, string s2, string s3)
// konkatenace retezcu
{
    if (strCopyString(s3, s1) != STR_SUCCESS)
        return STR_ERROR;
    // zkopirujeme si zacatek retezcu
    int lenght_s2 = strGetLength(s2);
    for (int i = 0 ; i < lenght_s2; i++)
    {
        if (strAddChar(s3, s2->str[i]) != STR_SUCCESS)
            return STR_ERROR;
    }
    return STR_SUCCESS;
}

int strCmpString(string s1, string s2)
// porovna oba retezce a vrati vysledek
{
   return strcmp(s1->str, s2->str);
}

int strCmpConstStr(string s1, const char* s2)
// porovna nas retezec s konstantnim retezcem
{
   return strcmp(s1->str, s2);
}

char *strGetStr(string s)
// vrati textovou cast retezce
{
   return s->str;
}

int strGetLength(string s)
// vrati delku daneho retezce
{
   return s->length;
}

void strReverse(string s)
{
	char xchgchar;
	int indexer = 0;

	if( !s->length ) return;

	while( indexer < s->length/2 )
	{
		xchgchar = s->str[indexer];
		s->str[indexer] = s->str[s->length - (indexer+1)];
		s->str[s->length - (indexer+1)] = xchgchar;
		indexer++;
	}
}

string strWrite(const char* conststr)
{
	string s = newString();
	int len = strlen(conststr);
	len++;
	
	if (len > s->allocSize)
   {
      // pamet nestaci, je potreba provest realokaci
      if ((s->str = (char*) realloc(s->str, len)) == NULL)
         return NULL;
      s->allocSize = len;
   }
   s->length = len -1;
   strcpy(s->str, conststr);
   
   return s;
	
}

int strtoint(string s)
// vlastni funkce na konverzi stringu na int
{
    return atoi(s->str);
}

double strtodouble(string s)
// vlastni funkce na konverzi stringu na double
{
    return atof(s->str);
}

/* konec str.c */

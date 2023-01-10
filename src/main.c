
/***************************************************
** PROJEKT do predmetu IFJ a IAL
** Encoding:  ISO-8859-2
** Nazev:     Implementace interpretu imperativniho jazyka IFJ10
** varianta:  Tym 6, varianta b/2/I
** resitele:
**
**  xnemec53 - Nemec Miroslav
**  xlange02 - Langer Lukáš
**  xmesza03 - Mészáros Tomáš
**  xmolna02 - Molnár Dávid
**  xpopto01 - Pop Tomáš
**
*****************************************************
**/

#include <stdio.h>
#include "globals.h"
#include "interpret.h"
#include "parser.h"


//////// CHYBOVE HLASKY ////////

char *ERRORS[] =
{
    /* OK */
    "OK",
    /* LEX_ERROR */
    "Nastala lexikalni chyba pri analyze zdrojoveho programu!",
    /* SYNTAX_ERROR */
    "Nastala syntakticka chyba pri analyze programu!",
    /* SEM_ERROR */
    "Nastala chyba pri semanticke kontrole programu!",
    /* RUNTIME_ERROR */
    "Nastala chyba pri interpretaci programu!",
    /* INTERNAL_ERROR */
    "Nastala interni chyba!",
};

// Globální seznam tabulek symbolů
tList STableList;
// Globální tabulka symbolů
//tSymbolTable STable;
//zdrojový soubor
FILE *srcFile = NULL;
//Seznam instrukcí
tInstructionList IList;
// chybovy kod
int ERROR_CODE;


////////////////////////////////
///////// FUNKCE MAIN //////////
////////////////////////////////

int main(int argc, char *argv[])
{

    enum ERROR_NUMBERS return_val = OK;

    if (argc == 1)
    { // kontrola parametru
        fprintf(stderr, "%s: nelze otevřít soubor\n", ERRORS[INTERNAL_ERROR] );
        return INTERNAL_ERROR;
    }
    if ((srcFile = fopen(argv[1], "r")) == NULL)
    { // kontrola otevriti souboru
        fprintf(stderr, "%s: nelze otevřít soubor\n", ERRORS[INTERNAL_ERROR] );
        return INTERNAL_ERROR;
    }

    initAllResources();

    return_val = parse();
    if( return_val != OK )
    {
        fprintf(stderr, "radek: %d, token: %s\n", line_num, token.attribut->str );
        printError(return_val);
        freeAllResources();
        return return_val;
    }

    #ifndef INSTRUCTIONS_OFF
    return_val = interpret();
    printError(return_val);
    #endif

    freeAllResources();

    return return_val;
}

/////// DEFINICE FUNKCII ////////

void printError(enum ERROR_NUMBERS errnum)
{
    if ( errnum > INTERNAL_ERROR)
    {
        errnum = INTERNAL_ERROR;
    }

    if (errnum != OK)
    {
        fprintf(stderr, "%s\n", ERRORS[errnum]);
        //exit(errnum);
    }
}

void initAllResources(void)
{
    tSymbolTable GST;
    initStrCollector();

    // inicializace seznamu TS
    InitList(&STableList);

    // nasledne inicializace globalni tabulky symbolu
    TableInit(&GST);

    //vlozeni globalni TS do seznamu tabulek symbolu
    InsertFirst(&STableList, GST);

    // nastaveni aktivity na globalni tabulku
    Last(&STableList);

    // inicializace seznamu instrukcii
    ListInit(&IList);

    setInstructionList(&IList);
}

void freeAllResources(void)
{
    // odalokovani seznamu a tabulky symbolu
    Dispose_List_Table(&STableList);

    deleteStrCollector();

    ListFree(&IList);

	fclose(srcFile);
}

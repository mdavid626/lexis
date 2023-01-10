/*****************************************************
** Soubor:    parser.c
** Encoding:  ISO-8859-2
** Datum:     2010/14/11
** Autor:     Tomáš Mészáros, xmesza03
** Projekt:   IAL, IFJ
** Nazev:     Implementace interpretu imperativniho jazyka IFJ10
** Popis:     zdrojovy kod Syntaktickeho analyzatoru.
*****************************************************
**/

#include "parser.h"
#include "precedence_parser.h"
#include "interpret.h"
#include "globals.h"
#include <assert.h>



TLex token;
unsigned int param_num = 0;
int offset = 0;
tData* prev_inputid = NULL;

/* Markra a funkce pro jednoduchou praci ************************************ */
/*                                                                           */

#define GET_NEXT_TOKEN() if((ret=getNextToken(srcFile, &token))==LEX_ERROR)break

#define GST STableList.Last->table

#define SEARCH_GST(string, flag) TableSearch(STableList.Last->table,string,flag)
#define SEARCH_LST(string, flag) TableSearch(STableList.Act->table,string, flag)

#define INSERT_GST(string, typ) TableInsert(&STableList.Last->table,string,typ)
#define INSERT_LST(string, typ) TableInsert(&STableList.Act->table, string, typ)

#define E_SEMANTIC() { ret = SEM_ERROR; break; }
#define E_SYNTAX() { ret = SYNTAX_ERROR; break; }
#define E_INTERNAL() { ret = INTERNAL_ERROR; break; }

tElemPtr newLocTab(void)
{
	if( InsertFirst(&STableList, NULL) == MALLOC_ERROR ) return NULL;
	else return STableList.First;
}

int areFunctionsDefined(tSymbolTable table)
{
    if ( table != NULL )
	{
        if( table->data.type == T_FUNCTION)
            if( !table->data.value.function.defined ) return FALSE;

		if( table->left != NULL )
            if( areFunctionsDefined( table->left ) == FALSE) return FALSE;
		if( table->right != NULL )
            if( areFunctionsDefined( table->right ) == FALSE) return FALSE;
	}

	return TRUE;
}

/* Deklarace funkci pro jednotlive pravidla ********************************* */
/*                                                                            */
int prog(void);
int decList(void);
int decl(void);
int def(void);
int type( tData* );	//ma doplnit typ funkce, nebo promenne
int paramList( tData* info );
int param(tData* info, int param_num);
int varList( void );
int var(void );
int itemList(void);
int statList(void);
int statement(void);
int exprList(void);
int elseBranch(void);
int expression(tData* expr_info);

/* Definice funkci  ********************************************************* */
/*                                                                            */


// ukazatele na operatory pro generate
//tData *op1, *op2, *op3;

int prog(void)
{
    int ret = SYNTAX_ERROR;
    string label = newString();
	//string findfunc = strWrite("FIND");
	//string sortstr = strWrite("SORT");


    newKey(label);
    tData* label_main = NULL;
	tData* builtin_func = NULL;

    while (token.type == END_OF_LINE) GET_NEXT_TOKEN();

    switch (token.type)
    {
        case DECLARE:
        case FUNCTION:
        case SCOPE:

        /* pravidlo 1:
         * <prog> -> <decList> scope EOL <varList> <statList> end scope EOL */

		#ifndef SEM_OFF
			builtin_func = INSERT_GST(strWrite("FIND"), T_FUNCTION);
			if ( builtin_func == NULL ) E_INTERNAL();
			builtin_func->value.function.declared = TRUE;
			builtin_func->value.function.defined = TRUE;
			builtin_func->value.function.localTable = STableList.Last;
			builtin_func->value.function.params = strWrite("iss");
			builtin_func->value.function.label = NULL;

			builtin_func = INSERT_GST(strWrite("SORT"), T_FUNCTION);
			if ( builtin_func == NULL ) E_INTERNAL();
			builtin_func->value.function.declared = TRUE;
			builtin_func->value.function.defined = TRUE;
			builtin_func->value.function.localTable = STableList.Last;
			builtin_func->value.function.params = strWrite("ss");
			builtin_func->value.function.label = NULL;
		#endif

        #ifndef INSTRUCTIONS_OFF
            /* Vygenerovani skoku na navesti main */
            label_main = INSERT_GST(label, T_LABEL);
            if( label_main == NULL ) E_INTERNAL();
            generate(I_GOTO, NULL, NULL, label_main );
        #endif

            if( (ret = decList()) != OK ) break;
            if( token.type != SCOPE ) E_SYNTAX();

            offset = 1;

            GET_NEXT_TOKEN();
            if( token.type != END_OF_LINE ) E_SYNTAX();


        #ifndef SEM_OFF
            /* sem. kontrola, jestli vsechny deklarovane fce byly definovane.
             * Realizovane pruchodem globalne tabulky. */
            if( !areFunctionsDefined(GST) ) E_SEMANTIC();

            // Aktivna tabulka bude globalna
            Last(&STableList);
        #endif

        #ifndef INSTRUCTIONS_OFF
            generate(I_LABEL, label_main, NULL, NULL);
        #endif

            GET_NEXT_TOKEN();
            if( (ret = varList()) != OK ) break;
            if( (ret = statList()) != OK ) break;
            if(    token.type != END ) E_SYNTAX();

            GET_NEXT_TOKEN();
            if( token.type != SCOPE ) E_SYNTAX();

            GET_NEXT_TOKEN();
            if( token.type != END_OF_LINE ) E_SYNTAX();

            /* Musi nutne nasledovat konec souboru, jinak synt. chyba */
            GET_NEXT_TOKEN();
            if( token.type == END_OF_FILE ) ret = OK;
            else ret = SYNTAX_ERROR;

        #ifndef INSTRUCTIONS_OFF
            //instukce uspesneho ukonceni programu
            generate(I_STOP, NULL, NULL, NULL);
        #endif

            break;

        default: ret = SYNTAX_ERROR;
    }

    return ret;
}

int decList(void)
{
    int ret = SYNTAX_ERROR;

    switch (token.type)
    {
        case DECLARE:
        /* pravidlo 2:
         * <decList> -> <decl> EOL <decList> */
            if( (ret = decl()) != OK ) break;
            if( token.type != END_OF_LINE ) E_SYNTAX();


            GET_NEXT_TOKEN();
            ret = decList();

        break;

        case FUNCTION:
        /* pravidlo 3:
         * <decList> -> <def> EOL <decList> */
            if( (ret = def()) != OK ) break;
            if (token.type != END_OF_LINE ) E_SYNTAX();

            GET_NEXT_TOKEN();
            ret = decList();

        break;

        case SCOPE:
        /* pravidlo 4:
         * <decList> -> eps */
            ret = OK;
        break;

        default: ret = SYNTAX_ERROR;

    }

    return ret;
}

int decl(void)
{
    int ret = SYNTAX_ERROR;
	int found = 0;
	tData* func_info = NULL;
	string id = newString();



    switch (token.type)
    {
        case DECLARE:
        /* pravidlo 5:
         * <decl> -> declare function id ( <paramList> ) as <type>
         * Semantika:
         * Deklarujem funkci, takze kontrloa jestli neni shodni identifikator v GST.
         * Kdzy je, SEM_ERROR, jinak vlozime identifikator do GST. paramList()
         * musi doplinit informace o typech parametru, type() doplni navratovy typ.
         * */
            GET_NEXT_TOKEN();
            if( token.type != FUNCTION ) E_SYNTAX();

            GET_NEXT_TOKEN();
            if( token.type != ID ) E_SYNTAX()
            else
            {
                //semanticke akce:
            #ifndef SEM_OFF
				strCopyString(id, token.attribut);

				SEARCH_GST(id, &found);
                // pokud id funkce v TS uz existuje jde o sem. chybu
                if (found) E_SEMANTIC();

                // vlozeni do globalni TS
                func_info = INSERT_GST(id, T_FUNCTION );
				if( func_info == NULL ) E_INTERNAL();

                // vlozeni neznameho navratoveho typu ( type() to potom doplni
                strAddChar(func_info->value.function.params, 'u');
                func_info->value.function.declared = TRUE;
                // kombinace 10 ( viz param() )
            #endif
            }

            GET_NEXT_TOKEN();
            if( token.type != LEFT_BRACKET ) E_SYNTAX();

            GET_NEXT_TOKEN();
            if( (ret = paramList(func_info)) != OK ) break;
            if(    token.type != RIGHT_BRACKET ) E_SYNTAX();

            GET_NEXT_TOKEN();
            if( token.type != AS ) E_SYNTAX();

            GET_NEXT_TOKEN();
            ret = type( func_info );

        break;

        default: ret = SYNTAX_ERROR;
    }

    return ret;
}

int def(void)
{
    int ret = SYNTAX_ERROR;
	string id = newString();
	int found = 0;


	tData* func_info = NULL;
    switch (token.type)
    {
        case FUNCTION:
        /* pravidlo 6:
         * <decl> -> function id ( <paramList> ) as <type> EOL <varList> <statList> end function
         * Semanticke akce:
         * Kontrolovat unikatnost identifikatoru fce v glob. tabulce
         * Kdyz existuje deklarace fce, kontrolovat typy parametru, navratovou
         * hodnotu, jestli neshoduji SEM_ERROR. Jinak nastavit priznak definovanosti na TRUE
         *
         * Kdyz neexistuje deklarace, vytvorit lok. tabulku, a doplinit informace
         * (tj. implicitne deklarovat fce, ale priznak deklarovanosti zustava FALSE)
         * */

            offset = 1;
            GET_NEXT_TOKEN();
            if( token.type != ID ) E_SYNTAX()
            else
            {
                //semanticke akce
            #ifndef SEM_OFF
                strCopyString(id, token.attribut);

                func_info = SEARCH_GST(id, &found );

                if ( found )
                {
                    if(func_info->type != T_FUNCTION)
					{
						ret = SEM_ERROR;
						break;
					}
                    else if( func_info->value.function.defined )
                    {
                        ret = SEM_ERROR;
						break;
                    }
					else func_info->value.function.defined = TRUE;
                    //kombinace 11 ( viz param() )
                }
                else
                {
                    // Vlozit funkce do GST, (jakoby inplicitni deklarace)
                    // kombinace 00 ( viz param() )
					func_info = INSERT_GST(id, T_FUNCTION );
                    strAddChar(func_info->value.function.params, 'u');
                }
                // vygenerovani navesti pro funkcii

                #ifndef INSTRUCTION_OFF
                generate(I_FUNCTION, func_info, NULL, NULL);
                #endif

            #endif

            }

            #ifndef SEM_OFF
                // Vytvorit lok. tabulku fce
                if((func_info->value.function.localTable = newLocTab()) == NULL)
                {
                    ret = INTERNAL_ERROR;
                    break;
                }
                else // Nastaveni aktiviti na lok. tabulku funkce
                    STableList.Act = func_info->value.function.localTable;
            #endif

            GET_NEXT_TOKEN();
            if( token.type != LEFT_BRACKET )
            {
                ret = SYNTAX_ERROR;
                break;
            }

            GET_NEXT_TOKEN();
            // skontroluje shodu parametru, nebo ich doplni
            if( (ret = paramList(func_info)) != OK ) break;
            if(    token.type != RIGHT_BRACKET )
            {
                ret = SYNTAX_ERROR;
                break;
            }

            GET_NEXT_TOKEN();
            if( token.type != AS )
            {
                ret = SYNTAX_ERROR;
                break;
            }

            GET_NEXT_TOKEN();
            //To same s priznakom defined
            if( (ret = type(func_info)) != OK ) break;
            if(    token.type != END_OF_LINE )
            {
                ret = SYNTAX_ERROR;
                break;
            }

            #ifndef SEM_OFF
            func_info->value.function.defined = TRUE;
			#endif

            GET_NEXT_TOKEN();
            if( (ret = varList()) != OK ) break;
            if( (ret = statList()) != OK ) break;
            if(    token.type != END )
            {
                ret = SYNTAX_ERROR;
                break;
            }

            GET_NEXT_TOKEN();
            if( token.type != FUNCTION ) ret = SYNTAX_ERROR;

            GET_NEXT_TOKEN();
            ret = OK;
        break;

        default: ret = SYNTAX_ERROR;
    }

    return ret;
}

int type( tData* info)
{
	//assert( info != NULL);
    char c = '\0';
    enum data_types vartype = T_UNKNOWN;

    int ret = OK;
    switch (token.type)
    {
        case INTEGER:
		/* pravidlo 7:
         * <type> -> integer */
            c = 'i';
            vartype = T_INT;
            GET_NEXT_TOKEN();
        break;

		case DOUBLE:
		/* pravidlo 8:
         * <type> -> double */
            c = 'd';
            vartype = T_DOUBLE;
            GET_NEXT_TOKEN();
        break;

		case STRING:
		/* pravidlo 8:
         * <type> -> double */
            c = 's';
            vartype = T_STRING;
            GET_NEXT_TOKEN();
        break;


        default: ret = SYNTAX_ERROR;

    }
    #ifndef SEM_OFF
    if( ret == OK && info != NULL )
    {
        switch( info->type )
        {
            case T_FUNCTION:
            //doplnit nebo kontrolovat navratovy typ podle priznaku defined
                if( info->value.function.defined)
                { //kontrolovat
                    if( info->value.function.params->str[0] != c ) ret = SEM_ERROR;
                }
                else
                { //doplnit
                    if( info->value.function.params->str[0] == 'u' )
                        info->value.function.params->str[0] =  c;
                    else ret = INTERNAL_ERROR;
                }
            break;

            case T_LABEL:
            //nemoze nastat
                ret = SEM_ERROR;
            break;


            case T_UNKNOWN:
            //doplnit typ promenne
                info->type = vartype;
            break;

            default:
            //kontrolovat typ promenne
                if( info->type != vartype ) ret = SEM_ERROR;

        }
    }
    #endif
    return ret;
}

int paramList(tData* info)
{
    int ret = SYNTAX_ERROR;

    switch (token.type)
    {
        case ID:
        /* pravidlo 10:
         * <paramList> -> <param> <paramList> */
            param_num++;
            if( (ret = param(info, param_num)) != OK ||
                (ret = paramList(info)) != OK ) break;
            else ret = OK;
        break;

        case COMMA:
        /* pravidlo 11:
         * <paramList> -> , <paramList> */
            GET_NEXT_TOKEN();
            if( (ret = paramList(info)) != OK  ) break;
            else ret = OK;
        break;

        case RIGHT_BRACKET:
        /* pravidlo 12:
         * <paramList> -> eps */
            param_num = 0;
            ret = OK;
        break;

        default: ret = SYNTAX_ERROR;
    }

    return ret;
}

int param(tData* func_info, int param_num)
{
    int ret = SYNTAX_ERROR;
    tData* var_info = NULL;
    int found = FALSE;
    #ifndef SEM_OFF
    int declared = func_info->value.function.declared;
    int defined = func_info->value.function.defined;
    #endif
    switch (token.type)
    {
        case ID:
        /* pravidlo 13:
         * <param> -> id as <type>
         * Semantika:
         * postup je zvolen podle flagu declared a defined
         * declared defined
         *    0       0    : doplnit typy parametru a navr. typ, nazvy vlozit do
         *                   tabulky(s kontrolou existence) - implicitni deklarace
         *    0       1    : by nemnel nastat (INTERNAL_ERROR)
         *    1       0    : doplnit typy parametru a navratove hodnoty
         *    1       1    : kontrolovat nazvy a typy declarace a definice
         * */
            #ifndef SEM_OFF

            if( !declared && !defined )
            { //00
                // parameter nemuze byt stejne jako nazev jine funkce
                var_info = SEARCH_GST( token.attribut, &found );
                if( found && var_info->type == T_FUNCTION )
                {   // jmeno parametru fce nemuze byt stejne jako nazev jine fce
                    ret = SEM_ERROR;
                    break;
                }

                var_info = SEARCH_GST( token.attribut, &found );
                if(found )
                {
                    if(var_info->type == T_FUNCTION)
                    {// Nazev lok. promenne (ci parametru) nemuze byt stejne
                     // jako nazev jine funkce
                        ret = SEM_ERROR;
                        break;
                    }
                }
                var_info = SEARCH_LST( token.attribut, &found );
                if( found )
                {
                    ret = SEM_ERROR;
                    break;
                }

                var_info = INSERT_LST( token.attribut, T_UNKNOWN);
            }
            else if( !declared && defined )
            { //01
                  ret = INTERNAL_ERROR;
                  break;
            }
            else if ( declared && !defined )
            { //10
                tData temp;
                temp.type = T_UNKNOWN;
                var_info = &temp;
            }
            else
            { //11

                var_info = SEARCH_GST( token.attribut, &found );
                if(found )
                {
                    if(var_info->type == T_FUNCTION)
                    {// Nazev lok. promenne (ci parametru) nemuze byt stejne
                     // jako nazev jine funkce
                        ret = SEM_ERROR;
                        break;
                    }
                }
                var_info = SEARCH_LST( token.attribut, &found );
                if( found )
                {
                    ret = SEM_ERROR;
                    break;
                }

                var_info = INSERT_LST( token.attribut, T_UNKNOWN);
            }
            #endif

            GET_NEXT_TOKEN();
            if( token.type != AS ) E_SYNTAX();

            GET_NEXT_TOKEN();
            if( (ret = type(var_info)) != OK ) break;

            #ifndef SEM_OFF
            if( !defined )
            {
                switch(var_info->type)
                {
                    case T_INT: strAddChar(func_info->value.function.params, 'i'); break;
                    case T_DOUBLE: strAddChar(func_info->value.function.params, 'd'); break;
                    case T_STRING: strAddChar(func_info->value.function.params, 's'); break;
                    default: ret = INTERNAL_ERROR; break;
                }
            }
            else
            {
                switch(var_info->type)
                {
                    case T_INT:
                        if(func_info->value.function.params->str[param_num] != 'i')
                            ret = SEM_ERROR;
                    break;

                    case T_DOUBLE:
                        if(func_info->value.function.params->str[param_num] != 'd')
                            ret = SEM_ERROR;
                    break;

                    case T_STRING:
                        if(func_info->value.function.params->str[param_num] != 's')
                            ret = SEM_ERROR;
                    break;

                    default: ret = INTERNAL_ERROR; break;
                }
            }
            #endif
        break; //case ID

        default: ret = SYNTAX_ERROR;
    }

    return ret;
}

int varList(void)
{
    int ret = SYNTAX_ERROR;

    switch (token.type)
    {
        case DIM:
        /* pravidlo 14:
         * <varList> -> <var> EOL <varList> */
            if( (ret = var()) != OK ) break;

            if( token.type != END_OF_LINE )
            {
                ret = SYNTAX_ERROR;
                break;
            }

            GET_NEXT_TOKEN();
            ret = varList();
        break;

        case DO:
        case END:
        case ID:
        case IF:
        case PRINT:
        case INPUT:
        case RETURN:
        /* pravidlo 15:
         * <varList> -> eps */
            ret = OK;
        break;

        default: ret = SYNTAX_ERROR;
    }

    return ret;
}

int var(void)
{
    int ret = SYNTAX_ERROR;
    tData* var_info = NULL;
    int found = FALSE;
    string id = newString();

    switch (token.type)
    {
        case DIM:
        /* pravidlo 16:
         * <varList> -> dim id as <type> */
            GET_NEXT_TOKEN();
            if( token.type != ID )
            {
                ret = SYNTAX_ERROR;
                break;
            }
            else
            {
            #ifndef SEM_OFF
                strCopyString(id, token.attribut);

                //nazev promenne nemuze byt shodny s nazvem jine funkce
                var_info = SEARCH_GST(id, &found);
                if(found )
                {
                    if( STableList.Act == STableList.Act ||
                        var_info->type == T_FUNCTION )
                        E_SEMANTIC()

                }

                //nazev tez nesmi byt stejne jako nazev jine lok. promenne v
                //ramci funkce
                if( STableList.Act != STableList.Last )
                {
                    SEARCH_LST(id, &found);
                    if(found) E_SEMANTIC();
                }

                var_info = INSERT_LST(id, T_UNKNOWN);
            #endif
            }

            GET_NEXT_TOKEN();
            if( token.type != AS )
            {
                ret = SYNTAX_ERROR;
                break;
            }

            GET_NEXT_TOKEN();

            ret = type(var_info);
        #ifndef INSTRUCTIONS_OFF
            generate(I_ALLOC, var_info, NULL, NULL);
        #endif

        break;

        default: ret = SYNTAX_ERROR;
    }

    return ret;
}

int itemList(void)
{
    int ret = SYNTAX_ERROR;
    tData* id_info;
    int found = FALSE;

    switch (token.type)
    {
        case ID:
        /* pravidlo 17:
         * <itemList> -> id <itemList>
         * Semantika:
         * Kontrolujeme existenci identifikatoru v prisl. lok. tabulce a jestli
         * typ je jeden z moznych datovych typu (int, double, string)
         * */

          #ifndef SEM_OFF
            id_info = SEARCH_LST(token.attribut, &found);
            if(found)
            {
                if(id_info->type == T_FUNCTION || id_info->type == T_LABEL )
                    E_SEMANTIC();

                if( prev_inputid != NULL)
                    prev_inputid->value.variable.next_input = id_info;
                else
                    generate(I_READ, id_info, NULL, NULL);

                prev_inputid = id_info;
            }
            else E_SEMANTIC();

            /* generovani instrukce READ... */
          #endif

            GET_NEXT_TOKEN();
            ret = itemList();
        break;

        case COMMA:
        /* pravidlo 18:
         * <itemList> -> , <itemList> */
            GET_NEXT_TOKEN();
            ret = itemList();
        break;

        case END_OF_LINE:
        /* pravidlo 19:
         * <itemList> -> eps */
            prev_inputid = NULL;
            ret = OK;
        break;

        default: ret = SYNTAX_ERROR;
    }

    return ret;
}

int statList(void)
{
    int ret = SYNTAX_ERROR;

    switch (token.type)
    {
        case DO:
        case ID:
        case IF:
        case INPUT:
        case PRINT:
        case RETURN:
        /* pravidlo 20:
         * <statList> -> <statement> EOL <statList> */
             if( (ret = statement()) != OK ) break;
             if(    token.type != END_OF_LINE )
             {
                ret = SYNTAX_ERROR;
                break;
             }

             GET_NEXT_TOKEN();
             ret = statList();
         break;

        case END:
        case LOOP:
        case ELSE:
        /* pravidlo 21:
         * <statList> -> eps */
            ret = OK;
        break;

        default: ret = SYNTAX_ERROR;
    }

    return ret;
}

int statement(void)
{
    int ret = SYNTAX_ERROR;

    tData* id_info = NULL;
    tData* expr_info = NULL;
    int found = FALSE;
    string exprkey = newString();

    tData* label1 = NULL;
    tData* label2 = NULL;
    string label_name = newString();


    if (token.type == IF || token.type == DO)
    {
        newKey(label_name);
        label1 = INSERT_LST(label_name, T_LABEL);
        newKey(label_name);
        label2 = INSERT_LST(label_name, T_LABEL);
    }

    #ifndef SEM_OFF
    if( token.type != INPUT )
    {
        newKey(exprkey);
        expr_info = INSERT_LST(exprkey, T_UNKNOWN);
        #ifndef INSTRUCTIONS_OFF
        generate(I_ALLOC, expr_info, NULL, NULL);
        #endif
    }
    #endif

    switch (token.type)
    {

        case INPUT:
        /* pravidlo 22:
         * <statement> -> input ; <itemList> */
            GET_NEXT_TOKEN();
            if( token.type != SEMICOLON ) E_SYNTAX();

            GET_NEXT_TOKEN();
            ret = itemList();
        break;

        case PRINT:
        /* pravidlo 23:
         * <statement> -> print <expression> ; <exprList>
         * Semantika:
         * Vytvorit pom. promennu do tab. symbolu pro ukladani vysledku vyrazu
         * */

            GET_NEXT_TOKEN();
            if( (ret = expression(expr_info)) != OK ) break;

        #ifndef INSTRUCTIONS_OFF
            generate(I_WRITE, expr_info, NULL, NULL);
            /* generovani insrukce pro PRINT */
        #endif

            if( token.type != SEMICOLON ) E_SYNTAX();

            GET_NEXT_TOKEN();
            ret = exprList();
        break;

        case ID:
        /* pravidlo 24:
         * <statement> -> id = <expression> */
            // zpracovat id

        #ifndef SEM_OFF
            id_info = SEARCH_LST(token.attribut, &found);
            if(found)
            {
                if(id_info->type == T_FUNCTION || id_info->type == T_LABEL )
                    E_SEMANTIC();
            }
            else E_SEMANTIC();
        #endif

            GET_NEXT_TOKEN();
            if( token.type != EQ ) E_SYNTAX();

            GET_NEXT_TOKEN();
            if( (ret = expression(expr_info) ) != OK) break;

            // converze typu a instrukce pro =
            #ifndef SEM_OFF
                if( expr_info->type == id_info->type )
                {
                    #ifndef INSTRUCTIONS_OFF
                    generate(I_MOVE, expr_info, NULL, id_info );
                    #endif
                }
                else if( expr_info->type == T_INT && id_info->type == T_DOUBLE )
                {
                    #ifndef INSTRUCTIONS_OFF
                    generate(I_INT_TO_DOUBLE, id_info, NULL, expr_info);
                    #endif
                }
                else E_SEMANTIC();

            #endif


        break;

        case IF:
        /* pravidlo 25:
         * <statement> -> if <expression> then EOL <statList> <elseBranch> end if */
            GET_NEXT_TOKEN();
            if( (ret = expression(expr_info)) != OK ) break;

        #ifndef INSTRUCTIONS_OFF
            generate(I_NOT, expr_info, NULL, expr_info);
            generate(I_IF_GOTO, expr_info, NULL, label1);
        #endif

            /* generovani skoku */

            if( token.type != THEN ) E_SYNTAX();

            GET_NEXT_TOKEN();
            if( token.type != END_OF_LINE ) E_SYNTAX();

            GET_NEXT_TOKEN();
            if( (ret = statList()) != OK ) break;

        #ifndef INSTRUCTIONS_OFF
            generate(I_GOTO, NULL, NULL, label2);
            generate(I_LABEL, label1, NULL, NULL);
        #endif

            if( (ret = elseBranch()) != OK ) break;

        #ifndef INSTRUCTIONS_OFF
            generate(I_LABEL, label2, NULL, NULL);
        #endif

            if( token.type != END ) E_SYNTAX();

            GET_NEXT_TOKEN();
            if( token.type != IF ) E_SYNTAX();

            GET_NEXT_TOKEN();
            ret = OK;
        break;

        case DO:
        /* pravidlo 26:
         * <statement> -> do while <expression> EOL <statList> loop*/

         /* DOPLNIT INSTRUKCE */

            GET_NEXT_TOKEN();
            if( token.type != WHILE ) E_SYNTAX();

            #ifndef INSTRUCTIONS_OFF
                generate(I_LABEL, label2, NULL, NULL);
            #endif

            GET_NEXT_TOKEN();
            if( (ret = expression(expr_info)) != OK ) break;

            #ifndef INSTRUCTIONS_OFF
                generate(I_NOT, expr_info, NULL, expr_info);
                generate(I_IF_GOTO, expr_info, NULL, label1);
            #endif

            if( token.type != END_OF_LINE )
            {
                ret = SYNTAX_ERROR;
                break;
            }

            GET_NEXT_TOKEN();
            if( (ret = statList()) != OK ) break;

            //label1

            if( token.type != LOOP ) E_SYNTAX();

            #ifndef INSTRUCTIONS_OFF
                generate(I_GOTO, NULL, NULL, label2);
                generate(I_LABEL, label1, NULL, NULL);
            #endif

            GET_NEXT_TOKEN();
            ret = OK;
        break;


        case RETURN:
        /* pravidlo 27:
         * <statement> -> return <expression> */

		 if( STableList.Act == STableList.Last) E_SEMANTIC();

            GET_NEXT_TOKEN();
            if( (ret = expression(expr_info)) != OK) break;

        #ifndef INSTRUCTIONS_OFF
            generate(I_RETURN, expr_info, NULL, NULL);
        #endif

            /* generovani return */
        break;

        default: ret = SYNTAX_ERROR;
    }

    return ret;
}

int exprList(void)
{
    int ret = SYNTAX_ERROR;
    tData* expr_info = NULL;
    string exprkey = newString();

    switch (token.type)
    {
        case END_OF_LINE:
        /* pravidlo 29:
         * <exprList> -> eps */
            ret = OK;
        break;

        default:
        /* pravidlo 28 ( jakoby case EXPRESSION ):
         * <exprList> -> <expression> ; <exprList>
         * Tento parser s neterminalem <expression> uz nepracuje.
         * Podle pravidla 32: <expression> -> EXPRESSION by to derivoval na
         * terminalny symbol, ale v skutecnosti necha zbytek prace na
         * precedencnu synt. analyzu */

            #ifndef SEM_OFF
            if( token.type != INPUT )
            {
                newKey(exprkey);
                expr_info = INSERT_LST(exprkey, T_INT);
            }
            #endif

            #ifndef INSTRUCTIONS_OFF
                generate(I_ALLOC, expr_info, NULL, NULL);
            #endif

            if( (ret = expression(expr_info)) != OK ) break;
            /* genervani instrukce PRINT */
        #ifndef INSTRUCTIONS_OFF
            generate(I_WRITE, expr_info, NULL, NULL);
        #endif

            if( token.type != SEMICOLON ) E_SYNTAX();

            GET_NEXT_TOKEN();
            ret = exprList();
    }

    return ret;
}

int elseBranch(void)
{
    int ret = SYNTAX_ERROR;

    switch (token.type)
    {
        case ELSE:
        /* pravidlo 30:
         * <elseBranch> -> else EOL <statList> */
            GET_NEXT_TOKEN();
            if( token.type != END_OF_LINE )
            {
                ret = SYNTAX_ERROR;
                break;
            }

            GET_NEXT_TOKEN();
            ret = statList();
        break;

        case END:
        /* pravidlo 31:
         * <elseBranch> -> eps */
            ret = OK;
        break;

        default: ret = SYNTAX_ERROR;
    }

    return ret;

}

int expression(tData* expr_info)
{/* pakovaci funkce pro vyrazy ktera bude volat expressionParser. Ma smysl pro
  * testovani */

#ifndef EXP_PARSER_OFF
    return expresionParser(expr_info);
#else
    expr_info = NULL;
    do
    {/* volame tokeny kym nenarazi na jeden z FOLLOW(<expression>), aby bylo
      * mozne otestovat funkcnost bez expressionParser-u */
        if( getNextToken(srcFile, &token) == LEX_ERROR ) return LEX_ERROR;
    } while ( token.type != SEMICOLON && token.type != END_OF_LINE &&
              token.type != THEN );

    return OK;
#endif
}

int parse()
{
    token = tokenInit();

    int ret = getNextToken(srcFile, &token);

    if( ret == OK )
    {
        ret = prog();
    }

    return ret;
}

void newKey(string s)
{
    static unsigned int counter = 0;
    //unsigned int reminder = 0;
    unsigned int result = counter;

    strClear(s);
	strAddChar(s,'@');
    do
    {
        strAddChar(s, (char)(result % 10 + '0'));
        result /= 10;
    }while (result != 0);

	//strReverse(s);

    counter++;
}

/* Jeste zbyva:
 * doplnit instrukce pro input v itemList() */


/*****************************************************
** Soubor:    interpret.c
** Encoding:  ISO-8859-2
** Datum:     2010/30/11
** Autor:     Lukáš Langer, xlange02
** Projekt:   IAL, IFJ
** Nazev:     Implementace interpretu imperativniho jazyka IFJ10
** Popis:     Implementace interpretu vcetne generovani instrukcii
**            a jejich interpretaci
*****************************************************
**/

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <ctype.h>

#include "globals.h"
#include "interpret.h"
#include "ial.h"

#ifdef PRINT_INSTRUCTIONS
char* INSTR[] =
{
        // instrukce pro allokaci a deallokaci
    "I_ALLOC",

    // instrukce pro vlozeni a vybrani ze interpretovaciho zasobniku
    "I_PUSH",

    // aritmeticke instrukce
    "I_PLUS",
    "I_MINUS",
    "I_MULT",
    "I_DIV",

    // instrukce negace
    "I_NOT",

    // instrukce nepodmineneho skoku
    "I_GOTO",
    // instrukce podmineneho skoku
    "I_IF_GOTO",

    // instrukce volani funkce
    "I_CALL",
    // instrukce navratu z funkce
    "I_RETURN",

    // navesti
    "I_LABEL",
    // navesti pro funkci
    "I_FUNCTION",

    // vstupni a vystupni instrukce
    "I_PRE_READ",
    "I_READ",
    "I_WRITE",

    // instrukce konverzni
    "I_INT_TO_DOUBLE",

    // instrukce presunu
    "I_MOVE",

    // instrukce nahrani konstanty do promene
    "I_CONST",

    // instrukce vynulovani
    "I_ZERO",

    // instrukce testovaci
    "I_TEST_G",
    "I_TEST_GEQ",
    "I_TEST_L",
    "I_TEST_LEQ",
    "I_TEST_EQ",
    "I_TEST_NEQ",

    // vestavane funkce pro vyhledani a serazeni retezce
    "I_FIND",
    "I_SORT",

    // instrukce konce programu
    "I_STOP",
};
#endif

// zasobnik se bude postupne alokovat na nasobky tohohle cisla
#define STACK_INC 1000

// globalni ukazatel vramci modulu pro funkci generate
tInstructionList *L;

typedef struct t_stack_record
{
    void* ptr;
    int type;
} TStackRecord;

// struktura popisujici zasobnik pro interpret
typedef struct
{
    TStackRecord* a;
    //void **a;
    int BP;
    int SP;
    int alloc_size;
} tInterpretStack;

// struktura popisujici pomocny zasobnik na zalohu BP
typedef struct
{
    int a[MAX_STACK];
    int top;
} tIStack;

// struktura popisujici pomocny zasobnik na zalohu navratovych adres
typedef struct
{
    tListItem **a;
    int top;
    int alloc_size;
} tPtrStack;


#define S_INC 100

/////////////////////////////////////////////////////
///// prototypy pro implementace zasobniku
/////////////////////////////////////////////////////

int SInit(tInterpretStack *S);
int SPush(tInterpretStack *S, int type, void *ptr);
TStackRecord *SIndexTop(tInterpretStack *S, int I);
int SAlloc(tInterpretStack *S, int I, int type);
void SFree(tInterpretStack *S);
void freeTStackRecord( TStackRecord* rptr);
int SBackup(tInterpretStack *S, tIStack *IS, int counter);
void SRestore(tInterpretStack *S, tIStack *IS);
void SClear(tInterpretStack *S);

/////////////////////////////////////////////////////////////////
// prototypy pro implementaci pomocneho zasobniku na zalohu BP
/////////////////////////////////////////////////////////////////

void ISInit(tIStack *S);
int ISPush(tIStack *S, int val);
int ISPoptop(tIStack *S);

////////////////////////////////////////////////////////////////////////////////
// prototypy pro implementaci pomocneho zasobniku na zalohu navratovych adress
////////////////////////////////////////////////////////////////////////////////

int StackInit(tPtrStack *Stack);
void StackFree(tPtrStack *Stack);
int StackPush(tPtrStack *Stack, tListItem *Instruction);
tListItem *StackPopTop(tPtrStack *Stack);
int StackEmpty(tPtrStack *Stack);


/////////////////////////////////////////////////////////////
// definice funkcii pro pomocny zasobnik navratovych adress
/////////////////////////////////////////////////////////////

// inicializace zasobniku a naalokovani zakladni pameti pro zasobnik
int StackInit(tPtrStack *Stack)
{
    Stack->top = 0;
    Stack->alloc_size = STACK_INC;
    tListItem **temp = malloc(sizeof(struct list_item) * (Stack->alloc_size));
    if (temp == NULL)
    {
        return MALLOC_ERROR;
    }
    Stack->a = temp;
    return OK;
}

// zruseni zasobniku a uvolneni pameti
void StackFree(tPtrStack *Stack)
{
    free(Stack->a);
    Stack->a = NULL;
    Stack->top = 0;
}

// vlozeni noveho prvku na zasobnik
int StackPush(tPtrStack *Stack, tListItem *Instruction)
{
    if ((Stack->top + 1) >= Stack->alloc_size)
    {
        Stack->alloc_size = Stack->alloc_size + STACK_INC;
        Stack->a = realloc(Stack->a, (sizeof(struct list_item)*Stack->alloc_size));
        if (Stack->a == NULL)
        {
            return MALLOC_ERROR;
        }
    }
    Stack->top++;
    Stack->a[Stack->top] = Instruction;
    return OK;
}

// vybrani a odmazani hodnoty z vrcholu zasobniku
tListItem *StackPopTop(tPtrStack *Stack)
{
    if (Stack->top != 0)
    {
        return (Stack->a[Stack->top--]);
    }
    else
    {
        return NULL;
    }
}

// predikat na zjisteni prazdneho zasobniku
// jeli zasobnik prazdny vraci true jinak vraci false
int StackEmpty(tPtrStack *Stack)
{
    return (Stack->top == 0);
}

/////////////////////////////////////////////////////
//// definice funkcii pro pomocny zasobnik
/////////////////////////////////////////////////////

// inicializace pomocneho zasobniku
void ISInit(tIStack *S)
{
    S->top = -1;
}

// vlozeni hodnoty na pomocny zasobnik
int ISPush(tIStack *S, int val)
{
    S->top++;
    if (S->top != MAX_STACK)
    {
        S->a[S->top] = val;
        return OK;
    }
    else
        return INTERNAL_ERROR;
}

// vybrani hodnoty z pomocneho zasobniku
int ISPoptop(tIStack *S)
{
    assert(S->top >= 0);
    return S->a[S->top];
}

/////////////////////////////////////////////////////
//// definice funkcii pro programovy zasobnik
/////////////////////////////////////////////////////

// inicializace zasobniku pro interpret
int SInit(tInterpretStack *S)
{
    S->SP = -1;
    S->BP = -1;
    S->alloc_size = S_INC;
    TStackRecord* temp = malloc(sizeof(TStackRecord) * (S->alloc_size));
    if (temp == NULL)
    {
        return INTERNAL_ERROR;
    }
    S->a = temp;
    for (int i = 0; i < S->alloc_size; i++)
    { // vynulovani vsech ukazatelu v zasobniku
        S->a[i].ptr = NULL;
        S->a[i].type = T_UNKNOWN;
    }
    return OK;
}

// funkce na odallokovani vsech lokalnim promenych a parametru funkcii
// z vrcholu zasobniku az po base pointer
void SClear(tInterpretStack *S)
{
    while (S->SP != S->BP)
    {
        assert(S->SP >= 0);

        freeTStackRecord(&S->a[S->SP]);
        //S->a[S->SP] = NULL;
        S->SP--;
    }
}

// vlozeni ukazatele na zasobnik
int SPush(tInterpretStack *S, int type, void *ptr)
{


    if ((S->SP + 1) >= S->alloc_size)
    {
        S->alloc_size = S->alloc_size + S_INC;
        S->a = (TStackRecord*) realloc(S->a, (sizeof(TStackRecord)*S->alloc_size));
        if (S->a == NULL)
        {
            return INTERNAL_ERROR;
        }
        for (int i = S->SP+1; i < S->alloc_size; i++)
        { // vynulovani vsech ukazatelu v zasobniku
            S->a[i].ptr = NULL;
            S->a[i].type = type;
        }
    }

    void *temp;
    switch (type)
    {
        case T_INT:
            temp = (int*) malloc(sizeof(int));
            if (temp == NULL)
            {
                return INTERNAL_ERROR;
            }
            *(int *)temp = *(int*)ptr;
            break;

        case T_DOUBLE:
            temp = (double*) malloc(sizeof(double));
            if (temp == NULL)
            {
                return INTERNAL_ERROR;
            }
            *(double*)temp = *(double*)ptr;
            break;

        case T_STRING:
            temp = newString();
            strCopyString(temp, (string) ptr);
            break;
    }
    // zkopirovani hodnoty


    S->SP++;
    S->a[S->SP].ptr = temp;
    S->a[S->SP].type = type;
    return OK;
}

// funkce vyhradi na zasobniku misto
// misto na zasobniku je aktualni BP + I
int SAlloc(tInterpretStack *S, int I, int type)
{
	//printf("\n%d  %d\n", S->SP, S->BP);
    //assert((S->SP + 1) == (S->BP + I));
    if ((S->SP + 1) >= S->alloc_size)
    {
        S->alloc_size = S->alloc_size + S_INC;
        S->a = (TStackRecord*) realloc(S->a, (sizeof(TStackRecord)*S->alloc_size));
        if (S->a == NULL)
        {
            return INTERNAL_ERROR;
        }
        for (int i = S->SP+1; i < S->alloc_size; i++)
        { // vynulovani vsech ukazatelu v zasobniku
            S->a[i].ptr = NULL;
            S->a[i].type = type;
        }
    }

    void *temp;
    switch (type)
    {
        case T_INT:
            temp = (int*) malloc(sizeof(int));
            if (temp == NULL)
            {
                return INTERNAL_ERROR;
            }
            *(int*)temp = 0;
            break;

        case T_DOUBLE:
            temp = (double*)  malloc(sizeof(double));
            if (temp == NULL)
            {
                return INTERNAL_ERROR;
            }
            *(double*)temp = 0;
            break;

        case T_STRING:
            temp = newString();
            if (temp == NULL)
            {
                return INTERNAL_ERROR;
            }
            strClear(temp);
            break;
    }
    S->a[S->BP + I].ptr = temp;
    S->a[S->BP + I].type = type;
    S->SP = S->BP + I;
    return OK;
}

// pristup k zadanemu indexu pomoci SP
TStackRecord* SIndexTop(tInterpretStack *S, int I)
{
    //assert((S->SP+1) >= I);
    return &S->a[S->BP + I];
}

// uvolneni programoveho zasobniku
void SFree(tInterpretStack *S)
{
    while (S->SP != -1)
    {
        assert(S->SP >= 0);

        freeTStackRecord(&S->a[S->SP]);
        //S->a[S->SP] = NULL;
        S->SP--;
    }
    free(S->a);
}

void freeTStackRecord( TStackRecord* rptr)
{
	//assert( rptr != NULL);
    if( rptr != NULL)
    {

		//assert( rptr->type != T_UNKNOWN);

        switch (rptr->type)
		{
			case T_INT:
				free((int*) rptr->ptr); break;

			case T_DOUBLE:
				free( (double*) rptr->ptr); break;

			default: break; //strFree( (string) rptr->ptr);
		}

		//free(rptr);
    }
}

// funkce pro zalohovani BP do pomocneho zasobniku
int SBackup(tInterpretStack *S, tIStack *IS, int counter)
{
    if (ISPush(IS, S->BP) == OK)
    {
        S->BP = S->SP - counter;
        return OK;
    }
    else
        return INTERNAL_ERROR;
}

void SRestore(tInterpretStack *S, tIStack *IS)
{
    S->BP = ISPoptop(IS);
}



int interpret(void)
{
    // nastaveni aktivity na prvi instrukcii
    ListFirst(L);
    // pomocny zasobnik pro ulozeni hodnot
    tPtrStack Sptr;
    // programovy zasobnik interpreta
    tInterpretStack S;
    // pomocny zasobnik pro zalohovani hodnot BP hlavniho zasobniku
    tIStack Sint;

    // inicializace ukazateloveho zasobniku
    if (StackInit(&Sptr) != OK)
        return INTERNAL_ERROR;
    // inicializace programoveho zasobniku
    if (SInit(&S) != OK)
        return INTERNAL_ERROR;
    // inicializace pomocneho zasobniku
    ISInit(&Sint);

    // citac, ktery pocita kolikrat byla zavolana instrukce I_PUSH
    int push_counter = 0;

    // offset posunuti oproti BP, offset je ulozen v TS
    int offset1, offset2, offset3;

    tTriCode I;

    string id_temp;


    // cyklus interpretace
    while (1)
    {
        // nacteni instrukce
        I = GetInstruction(L);
        // dekodovani instrukce

        #ifdef PRINT_INSTRUCTIONS
            printf("\n%s > %p,  %p,  %p", INSTR[I.type], I.op1, I.op2, I.op3);

            fflush(stdout);
        #endif

        switch (I.type)
        {
            // instrukce navesti... nic se neprovadi
            case I_LABEL:
                break;

            // allokacni instrukce
            case I_ALLOC:
                if (SAlloc(&S, ((tData*)I.op1)->value.variable.offset, ((tData*)I.op1)->type) != OK)
                    return INTERNAL_ERROR;
                break;

            // instrukce vlozeni na zasobnik
            case I_PUSH:
                push_counter++;
                offset1 = ((tData*)I.op1)->value.variable.offset;

                if (SPush(&S, ((tData*)I.op1)->type, SIndexTop(&S, offset1)->ptr) != OK)

                    return INTERNAL_ERROR;
                //if (SPush(&S, I.op1) != OK)
                 //   return INTERNAL_ERROR;

                break;

            case I_CALL:
                // ulozeni nasledujici instrukce na pomocny zasobnik
                if (StackPush(&Sptr, L->Active) != OK)
                    return INTERNAL_ERROR;
                // skok vramci seznamu instrukcii na navesti ulozeno v TS
                ListGoto(L, ((tData*)I.op1)->value.function.label);
                if ( SBackup(&S, &Sint, push_counter) != OK)
                    return INTERNAL_ERROR;
                push_counter = 0;
                break;

            // instrukce navratu z funkce
            case I_RETURN:
                offset1 = ((tData*)I.op1)->value.variable.offset;
				#ifdef PRINT_INSTRUCTIONS
                printf(">>>return<<<\n");
				#endif
                fflush(stdout);
                    switch (((tData*)I.op1)->type)
                    { // zkonpirovani hodnoty z op1 na pozici udavanou BP... tam se nachadzi prostor na navratovou hodnotu
                        case T_INT:
                            *((int*)SIndexTop(&S, 0)->ptr) = *((int*)SIndexTop(&S, offset1)->ptr);
                            break;

                        case T_DOUBLE:
                            *((double*)SIndexTop(&S, 0)->ptr) = *((double*)SIndexTop(&S, offset1)->ptr);
                            break;

                        case T_STRING:
                            strCopyString((string)SIndexTop(&S, 0)->ptr, (string)SIndexTop(&S, offset1)->ptr);
                            break;

                        default:
                            break;
                }
                SClear(&S);
                SRestore(&S, &Sint);

                ListGoto(L, StackPopTop(&Sptr));

                break;

            // instrukce nepodmineneho skoku
            // skace se na navesti definovane op3
            case I_GOTO:
                ListGoto(L, ((tData*)I.op3)->value.label);
            break;

            // instrukce podmineneho skoku
            // skace v pripade true op1 (promena int_number)
            // skace na navesti definovane op3
            case I_IF_GOTO:
                offset1 = ((tData*)I.op1)->value.variable.offset;
                if (*((int*)SIndexTop(&S, offset1)->ptr) != 0)
                {
                    ListGoto(L, ((tData*)I.op3)->value.label);
                }
            break;

            // instrukce nacteni int cisla, double cisla, nebo retezce, podle typu hodnoty
            case I_READ:
                offset1 = ((tData*)I.op1)->value.variable.offset;
                id_temp = newString();
                // vypsani ? na stdout
                printf("? ");
                char c;
                int type = ((tData*)I.op1)->type;
                int state = 0;
                int jump = 0;
                int read_type = T_UNKNOWN;

                while (1)
                {
                    c = getchar();

                    if (jump == 1)
                    break;

                    switch (state)
                    {
                        // hlavni stav
                        case 0:
                            if (c == ' ' || c == '\t')
                            {
                                state = 0;
                            }
                            else if (c == '"')
                                state = 1;
                            else if (isdigit(c) || c == '+' || c == '-')
                            {
                                state = 3;
                                strAddChar(id_temp, c);
                            }
                            else
                                return RUNTIME_ERROR;
                            break;

                        // stav pro nacteni stringu
                        case 1:
                            if (c == '\n')
                                return RUNTIME_ERROR;
                            else if (c == '\\')
                                state = 2;
                            else if (c == '"')
                            {
                                c = getchar();
                                read_type = T_STRING;
                                type = ((tData*)I.op1)->type;
                                if (type == read_type)
                                {
                                    strCopyString((string)SIndexTop(&S, offset1)->ptr, id_temp);
                                    strClear(id_temp);
                                    if (((tData*)I.op1)->value.variable.next_input == NULL)
                                    { // konec inputu
                                        state = 9;
                                    }
                                    else
                                    { // pokracujeme dale
                                        state = 0;
                                        I.op1 = ((tData*)I.op1)->value.variable.next_input;
                                        offset1 = ((tData*)I.op1)->value.variable.offset;
                                    }
                                    if (c == '\n')
                                    {
                                        jump = 1;
                                    }
                                    ungetc(c,stdin);

                                }
                                else
                                {
                                    return RUNTIME_ERROR;
                                }
                            }
                            else
                            {
                                strAddChar(id_temp, c);
                            }
                            break;

                        // stav pro nacteni escape sekvencii
                        case 2:
                            if( c == '"' || c == '\\')
                                strAddChar(id_temp, c);
                            else if (c == 't')
                                strAddChar(id_temp, '\t');
                            else if (c == 'n')
                                strAddChar(id_temp, '\n');
                            else
                                strAddChar(id_temp, c);
                            state = 1;
                            break;

                        // stav pro nacteni cisel
                        case 3:
                            if (isdigit(c))
                            {
                                state = 3;
                                strAddChar(id_temp, c);
                            }
                            else if (c == '.')
                            {
                                state = 4;
                                strAddChar(id_temp, c);
                            }
                            else if (c == 'e' || c == 'E')
                            {
                                state = 5;
                                strAddChar(id_temp, c);
                            }
                            else if (c == '+' || c == '-')
                            {
                                state = 6;
                                strAddChar(id_temp, c);
                            }
                            else if (c == ' ' || c == '\t' || c == '\n')
                            {

                                read_type = T_INT;
                                if (type == read_type || type == T_DOUBLE)
                                {
                                    type = ((tData*)I.op1)->type;
                                    if (type == T_DOUBLE)
                                    {
                                        *((double*)SIndexTop(&S, offset1)->ptr) = strtodouble(id_temp);
                                    }
                                    else
                                    {
                                        *((int*)SIndexTop(&S, offset1)->ptr) = strtoint(id_temp);
                                    }
                                    strClear(id_temp);
                                    if (((tData*)I.op1)->value.variable.next_input == NULL)
                                    { // konec inputu
                                        state = 9;
                                    }
                                    else
                                    { // pokracujeme dale
                                        state = 0;
                                        I.op1 = ((tData*)I.op1)->value.variable.next_input;
                                        offset1 = ((tData*)I.op1)->value.variable.offset;
                                    }
                                    if (c == '\n')
                                    {
                                        jump = 1;
                                    }
                                    ungetc(c, stdin);

                                }
                                else
                                {
                                    return RUNTIME_ERROR;
                                }
                            }
                            else
                            {
                                return RUNTIME_ERROR;
                            }
                            break;

                        case 4:
                            if (isdigit(c))
                            {
                                state = 7;
                                strAddChar(id_temp, c);
                            }
                            else
                            {
                                return RUNTIME_ERROR;
                            }
                            break;

                        case 5:
                            if (isdigit(c))
                            {
                                state = 8;
                                strAddChar(id_temp, c);
                            }
                            else
                            {
                                return RUNTIME_ERROR;
                            }
                            break;

                        case 6:
                            if (c == 'e' || c == 'E')
                            {
                                state = 5;
                                strAddChar(id_temp, c);
                            }
                            else
                            {
                                return RUNTIME_ERROR;
                            }
                            break;

                        case 7:
                            if (isdigit(c))
                            {
                                state = 7;
                                strAddChar(id_temp, c);
                            }
                            else if (c == 'e' || c == 'E')
                            {
                                state = 5;
                                strAddChar(id_temp, c);
                            }
                            else if (c == ' ' || c == '\t' || c == '\n')
                            {
                                read_type = T_DOUBLE;
                                type = ((tData*)I.op1)->type;
                                if (type == read_type)
                                {
                                    *((double*)SIndexTop(&S, offset1)->ptr) = strtodouble(id_temp);
                                    strClear(id_temp);
                                    if (((tData*)I.op1)->value.variable.next_input == NULL)
                                    { // konec inputu
                                        state = 9;
                                    }
                                    else
                                    { // pokracujeme dale
                                        state = 0;
                                        I.op1 = ((tData*)I.op1)->value.variable.next_input;
                                        offset1 = ((tData*)I.op1)->value.variable.offset;
                                    }
                                    if (c == '\n')
                                    {
                                        jump = 1;
                                    }
                                    ungetc(c, stdin);
                                }
                                else
                                {
                                    return RUNTIME_ERROR;
                                }
                            }
                            else
                            {
                                return RUNTIME_ERROR;
                            }
                            break;

                        case 8:
                            if (isdigit(c))
                            {
                                state = 8;
                                strAddChar(id_temp, c);
                            }
                            else if (c == ' ' || c == '\t' || c == '\n')
                            {
                                read_type = T_DOUBLE;
                                type = ((tData*)I.op1)->type;
                                if (type == read_type)
                                {
                                    *((double*)SIndexTop(&S, offset1)->ptr) = strtodouble(id_temp);
                                    strClear(id_temp);
                                    if (((tData*)I.op1)->value.variable.next_input == NULL)
                                    { // konec inputu
                                        state = 9;
                                    }
                                    else
                                    { // pokracujeme dale
                                        state = 0;
                                        I.op1 = ((tData*)I.op1)->value.variable.next_input;
                                        offset1 = ((tData*)I.op1)->value.variable.offset;
                                    }
                                    if (c == '\n')
                                    {
                                        jump = 1;
                                    }
                                    ungetc(c, stdin);
                                }
                                else
                                {
                                    return RUNTIME_ERROR;
                                }
                            }
                            else
                            {
                                return RUNTIME_ERROR;
                            }
                            break;

                        case 9:
                            state = 9;
                            if (c == '\n')
                            {
                                jump = 1;
                                ungetc(c,stdin);
                            }
                            break;
                    }
                }

            break;

            // instrukce vypsani hodnoty op1 na stdout
            case I_WRITE:
                offset1 = ((tData*)I.op1)->value.variable.offset;
                if (((tData*)I.op1)->type == T_INT)
                {
                    printf("% d", *((int*)SIndexTop(&S, offset1)->ptr));
                }
                else if (((tData*)I.op1)->type == T_DOUBLE)
                {
                    printf("% g", *((double*)SIndexTop(&S, offset1)->ptr));
                }
                else if (((tData*)I.op1)->type == T_STRING)
                {
                    printf("%s", ((string)SIndexTop(&S, offset1)->ptr)->str);
                }
                fflush(stdout);
            break;


            // instrukce negace op3 = !op1;
            case I_NOT:
                offset1 = ((tData*)I.op1)->value.variable.offset;
                offset3 = ((tData*)I.op3)->value.variable.offset;
                if (*((int*)SIndexTop(&S, offset1)->ptr) == 0)
                {

                    *((int*)SIndexTop(&S, offset3)->ptr) = 1;
                }
                else
                {
                    *((int*)SIndexTop(&S, offset3)->ptr) = 0;
                }
            break;

            // instrukce presunu op3 = op1;
            case I_MOVE:
                offset1 = ((tData*)I.op1)->value.variable.offset;
                offset3 = ((tData*)I.op3)->value.variable.offset;
                if (((tData*)I.op1)->type == T_INT)
                {
                    *((int*)SIndexTop(&S, offset3)->ptr) = *((int*)SIndexTop(&S, offset1)->ptr);
                }
                else if (((tData*)I.op1)->type == T_DOUBLE)
                {
                    *((double*)SIndexTop(&S, offset3)->ptr) = *((double*)SIndexTop(&S, offset1)->ptr);
                }
                else if (((tData*)I.op1)->type == T_STRING)
                {
                    strCopyString((string)SIndexTop(&S, offset3)->ptr, (string)SIndexTop(&S, offset1)->ptr);
                }
            break;

            // instrukce nahrani konstanty
            case I_CONST:
                offset1 = ((tData*)I.op1)->value.variable.offset;
                if (((tData*)I.op1)->type == T_INT)
                {
                    *((int*)SIndexTop(&S, offset1)->ptr) = strtoint((string)I.op2);
                }
                else if (((tData*)I.op1)->type == T_DOUBLE)
                {
                    *((double*)SIndexTop(&S, offset1)->ptr) = strtodouble((string)I.op2);
                }
                else if (((tData*)I.op1)->type == T_STRING)
                {
                    strCopyString((string)SIndexTop(&S, offset1)->ptr, (string)I.op2);
                }
                break;

            // instrukce nastaveni op1 = 0;
            case I_ZERO:
                offset1 = ((tData*)I.op1)->value.variable.offset;
                if (((tData*)I.op1)->type == T_INT)
                {
                    *((int*)SIndexTop(&S, offset1)->ptr) = 0;
                }
                else if (((tData*)I.op1)->type == T_DOUBLE)
                {
                    *((double*)SIndexTop(&S, offset1)->ptr) = 0.0;
                }
            break;

            // aritmeticke instrukce popis instrukci:  op3 = op1 OPERACE op2;
            case I_PLUS:
                offset1 = ((tData*)I.op1)->value.variable.offset;
                offset2 = ((tData*)I.op2)->value.variable.offset;
                offset3 = ((tData*)I.op3)->value.variable.offset;
                // kontroluje se pouze jeden typ operandu, kontrolu vsech operandu na stejny typ resi semanticka analyza
                if (((tData*)I.op1)->type == T_INT)
                {
                    *((int*)SIndexTop(&S, offset3)->ptr) = *((int*)SIndexTop(&S, offset1)->ptr) + *((int*)SIndexTop(&S, offset2)->ptr);
                }
                else if (((tData*)I.op1)->type == T_DOUBLE)
                {
                    *((double*)SIndexTop(&S, offset3)->ptr) = *((double*)SIndexTop(&S, offset1)->ptr) + *((double*)SIndexTop(&S, offset2)->ptr);
                }
                else
                {
                    strAddString((string)SIndexTop(&S, offset1)->ptr, (string)SIndexTop(&S, offset2)->ptr, (string)SIndexTop(&S, offset3)->ptr);
                }
            break;

            case I_MINUS:
                offset1 = ((tData*)I.op1)->value.variable.offset;
                offset2 = ((tData*)I.op2)->value.variable.offset;
                offset3 = ((tData*)I.op3)->value.variable.offset;
                // kontroluje se pouze jeden typ operandu, kontrolu vsech operandu na stejny typ resi semanticka analyza
                if (((tData*)I.op1)->type == T_INT)
                {
                    *((int*)SIndexTop(&S, offset3)->ptr) = *((int*)SIndexTop(&S, offset1)->ptr) - *((int*)SIndexTop(&S, offset2)->ptr);
                }
                else if (((tData*)I.op1)->type == T_DOUBLE)
                {
                    *((double*)SIndexTop(&S, offset3)->ptr) = *((double*)SIndexTop(&S, offset1)->ptr) - *((double*)SIndexTop(&S, offset2)->ptr);
                }
            break;

            case I_MULT:
                offset1 = ((tData*)I.op1)->value.variable.offset;
                offset2 = ((tData*)I.op2)->value.variable.offset;
                offset3 = ((tData*)I.op3)->value.variable.offset;
                // kontroluje se pouze jeden typ operandu, kontrolu vsech operandu na stejny typ resi semanticka analyza
                if (((tData*)I.op1)->type == T_INT)
                {
                    *((int*)SIndexTop(&S, offset3)->ptr) = *((int*)SIndexTop(&S, offset1)->ptr) * *((int*)SIndexTop(&S, offset2)->ptr);
                }
                else if (((tData*)I.op1)->type == T_DOUBLE)
                {
                    *((double*)SIndexTop(&S, offset3)->ptr) = *((double*)SIndexTop(&S, offset1)->ptr) * *((double*)SIndexTop(&S, offset2)->ptr);
                }
            break;

            case I_DIV:
                offset1 = ((tData*)I.op1)->value.variable.offset;
                offset2 = ((tData*)I.op2)->value.variable.offset;
                offset3 = ((tData*)I.op3)->value.variable.offset;
                // kontroluje se pouze jeden typ operandu, kontrolu vsech operandu na stejny typ resi semanticka analyza
                if (((tData*)I.op1)->type == T_INT)
                {
                    if (*((int*)SIndexTop(&S, offset2)->ptr) == 0)
                    { // kontrola zda se nebude delit nulou
                        return RUNTIME_ERROR;
                    }
                    *((int*)SIndexTop(&S, offset3)->ptr) = *((int*)SIndexTop(&S, offset1)->ptr) / *((int*)SIndexTop(&S, offset2)->ptr);
                }
                else if (((tData*)I.op1)->type == T_DOUBLE)
                {
                    if (*((double*)SIndexTop(&S, offset2)->ptr) == 0)
                    { // kontrola zda se nebude delit nulou
                        return RUNTIME_ERROR;
                    }
                    *((double*)SIndexTop(&S, offset3)->ptr) = *((double*)SIndexTop(&S, offset1)->ptr) / *((double*)SIndexTop(&S, offset2)->ptr);
                }
            break;

            // konverzni instrukce ktera provadi konverzi op3 = (double)op1;
            case I_INT_TO_DOUBLE:
                offset1 = ((tData*)I.op1)->value.variable.offset;
                offset3 = ((tData*)I.op3)->value.variable.offset;
                *((double*)SIndexTop(&S, offset3)->ptr) = (double)*((int*)SIndexTop(&S, offset1)->ptr);

            break;

            // instrukce vyhledani podretezce v retezci
            // pouziva vestavenou funkci find
            // vraci v op3 0 alebo -1 zda se v op1 vyskytuje podretezec op2
            case I_FIND:
                offset1 = ((tData*)I.op1)->value.variable.offset;
                offset2 = ((tData*)I.op2)->value.variable.offset;
                offset3 = ((tData*)I.op3)->value.variable.offset;
                ((tData*)I.op3)->type = T_INT;
                *((int*)SIndexTop(&S, offset3)->ptr) = findStr(*(string*)SIndexTop(&S, offset1)->ptr, *(string*)SIndexTop(&S, offset2)->ptr);
            break;

            // instrukce serazeni stringu op1. Vysledek serezeni se ulozi do op3
            case I_SORT:
                offset1 = ((tData*)I.op1)->value.variable.offset;
                offset3 = ((tData*)I.op3)->value.variable.offset;
                *(((string*)SIndexTop(&S, offset3)->ptr)) = sortStr(*(string*)SIndexTop(&S, offset1)->ptr);
            break;

            // instrukce testu if (op1 > op2) 2 then op3 = 1 else op3 = 0;
            case I_TEST_G:
                offset1 = ((tData*)I.op1)->value.variable.offset;
                offset2 = ((tData*)I.op2)->value.variable.offset;
                offset3 = ((tData*)I.op3)->value.variable.offset;
                ((tData*)I.op3)->type = T_INT;          // nastaveni typu operandu op3 na int
                if (((tData*)I.op1)->type == T_INT)
                {
                    if (*((int*)SIndexTop(&S, offset1)->ptr) > *((int*)SIndexTop(&S, offset2)->ptr))
                        *((int*)SIndexTop(&S, offset3)->ptr) = 1;
                    else
                        *((int*)SIndexTop(&S, offset3)->ptr) = 0;
                }
                else if (((tData*)I.op1)->type == T_DOUBLE)
                {
                    if (*((double*)SIndexTop(&S, offset1)->ptr) > *((double*)SIndexTop(&S, offset2)->ptr))
                        *((int*)SIndexTop(&S, offset3)->ptr) = 1;
                    else
                        *((int*)SIndexTop(&S, offset3)->ptr) = 0;
                }
            break;

            // instrukce testu if (op1 >= op2) 2 then op3 = 1 else op3 = 0;
            case I_TEST_GEQ:
                offset1 = ((tData*)I.op1)->value.variable.offset;
                offset2 = ((tData*)I.op2)->value.variable.offset;
                offset3 = ((tData*)I.op3)->value.variable.offset;
                ((tData*)I.op3)->type = T_INT;          // nastaveni typu operandu op3 na int
                if (((tData*)I.op1)->type == T_INT)
                {
                    if (*((int*)SIndexTop(&S, offset1)->ptr) >= *((int*)SIndexTop(&S, offset2)->ptr))
                        *((int*)SIndexTop(&S, offset3)->ptr) = 1;
                    else
                        *((int*)SIndexTop(&S, offset3)->ptr) = 0;
                }
                else if (((tData*)I.op1)->type == T_DOUBLE)
                {
                    if (*((double*)SIndexTop(&S, offset1)->ptr) >= *((double*)SIndexTop(&S, offset2)->ptr))
                        *((int*)SIndexTop(&S, offset3)->ptr) = 1;
                    else
                        *((int*)SIndexTop(&S, offset3)->ptr) = 0;
                }
            break;

            // instrukce testu if (op1 < op2) 2 then op3 = 1 else op3 = 0;
            case I_TEST_L:
                offset1 = ((tData*)I.op1)->value.variable.offset;
                offset2 = ((tData*)I.op2)->value.variable.offset;
                offset3 = ((tData*)I.op3)->value.variable.offset;
                ((tData*)I.op3)->type = T_INT;          // nastaveni typu operandu op3 na int
                if (((tData*)I.op1)->type == T_INT)
                {
                    if (*((int*)SIndexTop(&S, offset1)->ptr) < *((int*)SIndexTop(&S, offset2)->ptr))
                        *((int*)SIndexTop(&S, offset3)->ptr) = 1;
                    else
                        *((int*)SIndexTop(&S, offset3)->ptr) = 0;
                }
                else if (((tData*)I.op1)->type == T_DOUBLE)
                {
                    if (*((double*)SIndexTop(&S, offset1)->ptr) < *((double*)SIndexTop(&S, offset2)->ptr))
                        *((int*)SIndexTop(&S, offset3)->ptr) = 1;
                    else
                        *((int*)SIndexTop(&S, offset3)->ptr) = 0;
                }
            break;

            // instrukce testu if (op1 <= op2) 2 then op3 = 1 else op3 = 0;
            case I_TEST_LEQ:
                offset1 = ((tData*)I.op1)->value.variable.offset;
                offset2 = ((tData*)I.op2)->value.variable.offset;
                offset3 = ((tData*)I.op3)->value.variable.offset;
                ((tData*)I.op3)->type = T_INT;          // nastaveni typu operandu op3 na int
                if (((tData*)I.op1)->type == T_INT)
                {
                    if (*((int*)SIndexTop(&S, offset1)->ptr) <= *((int*)SIndexTop(&S, offset2)->ptr))
                        *((int*)SIndexTop(&S, offset3)->ptr) = 1;
                    else
                        *((int*)SIndexTop(&S, offset3)->ptr) = 0;
                }
                else if (((tData*)I.op1)->type == T_DOUBLE)
                {
                    if (*((double*)SIndexTop(&S, offset1)->ptr) <= *((double*)SIndexTop(&S, offset2)->ptr))
                        *((int*)SIndexTop(&S, offset3)->ptr) = 1;
                    else
                        *((int*)SIndexTop(&S, offset3)->ptr) = 0;
                }
            break;

            // instrukce testu if (op1 == op2) 2 then op3 = 1 else op3 = 0;
            case I_TEST_EQ:
                offset1 = ((tData*)I.op1)->value.variable.offset;
                offset2 = ((tData*)I.op2)->value.variable.offset;
                offset3 = ((tData*)I.op3)->value.variable.offset;
                ((tData*)I.op3)->type = T_INT;          // nastaveni typu operandu op3 na int
                if (((tData*)I.op1)->type == T_INT)
                {
                    if (*((int*)SIndexTop(&S, offset1)->ptr) == *((int*)SIndexTop(&S, offset2)->ptr))
                        *((int*)SIndexTop(&S, offset3)->ptr) = 1;
                    else
                        *((int*)SIndexTop(&S, offset3)->ptr) = 0;
                }
                else if (((tData*)I.op1)->type == T_DOUBLE)
                {
                    if (*((double*)SIndexTop(&S, offset1)->ptr) == *((double*)SIndexTop(&S, offset2)->ptr))
                        *((int*)SIndexTop(&S, offset3)->ptr) = 1;
                    else
                        *((int*)SIndexTop(&S, offset3)->ptr) = 0;
                }
            break;

            // instrukce testu if (op1 != op2) 2 then op3 = 1 else op3 = 0;
            case I_TEST_NEQ:
                offset1 = ((tData*)I.op1)->value.variable.offset;
                offset2 = ((tData*)I.op2)->value.variable.offset;
                offset3 = ((tData*)I.op3)->value.variable.offset;
                ((tData*)I.op3)->type = T_INT;          // nastaveni typu operandu op3 na int
                if (((tData*)I.op1)->type == T_INT)
                {
                    if (*((int*)SIndexTop(&S, offset1)->ptr) != *((int*)SIndexTop(&S, offset2)->ptr))
                        *((int*)SIndexTop(&S, offset3)->ptr) = 1;
                    else
                        *((int*)SIndexTop(&S, offset3)->ptr) = 0;
                }
                else if (((tData*)I.op1)->type == T_DOUBLE)
                {
                    if (*((double*)SIndexTop(&S, offset1)->ptr) != *((double*)SIndexTop(&S, offset2)->ptr))
                        *((int*)SIndexTop(&S, offset3)->ptr) = 1;
                    else
                        *((int*)SIndexTop(&S, offset3)->ptr) = 0;
                }
            break;

            // instrukce konce programu
            case I_STOP:
                SFree(&S);
                StackFree(&Sptr);
                return OK;
            break;
            // dalsi instrukce

            default:
            break;
        }
        ListNext(L);
    }

    return OK;
}


void setInstructionList(tInstructionList *List)
{
    L = List;
}


void generate(int type, void *op1, void *op2, void *op3)
{
    //printf("\n%s > %p,  %p,  %p", INSTR[type], op1, op2, op3);
    //fflush(stdout);
    if (type == I_LABEL)
    {
        tTriCode temp = {type ,NULL ,NULL ,NULL};
        ((tData*)op1)->value.label = ListGetLast(L);
        ListInsertLast(L, temp);
    }
    else if (type == I_FUNCTION)
    {
        tTriCode temp = {type ,NULL ,NULL ,NULL};
        ((tData*)op1)->value.function.label = ListGetLast(L);
        ListInsertLast(L, temp);
    }
    else
    {
        tTriCode temp = {type, op1, op2, op3};
        ListInsertLast(L, temp);
    }
}

/* konec interpret.c */

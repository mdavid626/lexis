
/*****************************************************
** Soubor:    precedence_parser.c
** Encoding:  ISO-8859-2
** Datum:     2010/14/11
** Autor:     Tomáš Pop, xpopto01
** Projekt:   IAL, IFJ
** Nazev:     Implementace interpretu imperativniho jazyka IFJ10
** Popis:     zdrojovy kod Precedencniho parseru (zdola->nahoru)
*****************************************************
**/

#include <string.h>
#include "precedence_parser.h"
#include "globals.h"
#include "scaner.h"
#include "parser.h"
#include "str.h"
#include "interpret.h"

#include <assert.h>

#define TABLE_LENGTH 19
#define STACK_LIMIT 500  // zvetsovani zasobniku po 500 prvcich
#define OPERAND_STACK_LIMIT 15
#define EXPRESSION 1
//#define TRUE 1
//#define FALSE 0

struct t_function *p_function = NULL; // kdyz narazim na zpracovani funkce, tak zde si ulozim ukazatel na vlastnosti
int parameter_index = 0; // na kolikatym parametru funkce prave jsem - 0 navratova hodnota funkce

//////////////////////////// Precedencni tabulka a pravidla /////////////////////////////////

//hodnoty ulozene v precedencni tabulce - znamenaji akci
enum TABLE_VALUE{
  P = 50, // Push zacinam od teto hodnoty abych mohl porovnavat retezce '\0' a kvuli kolizi enumu
  R, // Reduce
  I, // Insert
  E, // Error
  S  // Succes - end
};


// implementace precedencni tabulky
const int prec_table[TABLE_LENGTH][TABLE_LENGTH]={
// i c_ic_dc_s +  -  * c.d /  = <>  > >=  < <=  (  )  ,  EOL  /////////////////token
  {E, E, E, E, R, R, R, R, R, R, R, R, R, R, R, I, R, R, R}, // id
  {E, E, E, E, R, R, R, R, R, R, R, R, R, R, R, E, R, R, R}, // c_int
  {E, E, E, E, R, R, R, R, R, R, R, R, R, R, R, E, R, R, R}, // c_d.
  {E, E, E, E, R, R, R, R, R, R, R, R, R, R, R, E, R, R, R}, // c_str
  {P, P, P, P, R, R, P, P, P, R, R, R, R, R, R, P, R, E, R}, // plus
  {P, P, P, P, P, R, P, P, P, R, R, R, R, R, R, P, R, E, R}, // minus
  {P, P, P, P, R, R, R, R, R, R, R, R, R, R, R, P, R, E, R}, // krat
  {P, P, P, P, R, R, P, R, P, R, R, R, R, R, R, P, R, E, R}, // celociselne deleni
  {P, P, P, P, R, R, P, R, R, R, R, R, R, R, R, P, R, E, R}, // deleni
  {P, P, P, P, P, P, P, P, P, R, R, R, R, R, R, P, R, E, R}, // rovno
  {P, P, P, P, P, P, P, P, P, P, R, R, R, R, R, P, R, E, R}, // nerovno
  {P, P, P, P, P, P, P, P, P, P, P, R, R, R, R, P, R, E, R}, // mensi
  {P, P, P, P, P, P, P, P, P, P, P, P, R, R, R, P, R, E, R}, // mensi rovno
  {P, P, P, P, P, P, P, P, P, P, P, P, P, R, R, P, R, E, R}, // vetsi
  {P, P, P, P, P, P, P, P, P, P, P, P, P, P, R, P, R, E, R}, // vetsi rovno
  {P, P, P, P, P, P, P, P, P, P, P, P, P, P, P, P, I, P, E}, // leva zavorka
  {E, E, E, E, R, R, R, R, R, R, R, R, R, R, R, E, R, E, R}, // prava zavorka
  {P, P, P, P, E, E, E, E, E, E, E, E, E, E, E, E, R, R, E}, // carka
  {P, P, P, P, P, P, P, P, P, P, P, P, P, P, P, P, E, E, S}  // konec radku
};

// implementace 19-ti pravidel
// vsechny vyrazy nize se prepisuji na EXPRESSION - staci jen co se muye prepsat
const char rules[19][5] = {
  {ID, LEFT_BRACKET, RIGHT_BRACKET, '\0'}, // E->i()
  {ID, LEFT_BRACKET, EXPRESSION, RIGHT_BRACKET, '\0'}, // E->i(E)
  {EXPRESSION, COMMA, EXPRESSION, '\0'}, // E->E,E
  {EXPRESSION, EQ, EXPRESSION, '\0'}, // E->E=E
  {EXPRESSION, NOT_EQ, EXPRESSION, '\0'}, // E->E<>E
  {EXPRESSION, LEFT_LESS, EXPRESSION, '\0'}, // E->E<E
  {EXPRESSION, LEFT_LESS_EQ, EXPRESSION, '\0'}, //E->E<=E
  {EXPRESSION, RIGHT_LESS, EXPRESSION, '\0'}, // E->E>E
  {EXPRESSION, RIGHT_LESS_EQ, EXPRESSION, '\0'}, // E->E>=E
  {EXPRESSION, PLUS, EXPRESSION, '\0'}, // E->E+E
  {EXPRESSION, MINUS, EXPRESSION, '\0'}, // E->E-E
  {EXPRESSION, MULT, EXPRESSION, '\0'}, // E->E*E
  {EXPRESSION, DIV_DOUBLE, EXPRESSION, '\0'}, // E->E/E
  {EXPRESSION, DIV_INT, EXPRESSION, '\0'}, // E->E\E celociselne deleni
  {LEFT_BRACKET, EXPRESSION, RIGHT_BRACKET, '\0'}, // E->(E)
  {ID, '\0'}, // E->i
  {CONST_INT, '\0'}, // E->c_int
  {CONST_DOUBLE, '\0'}, // E->c_d.
  {CONST_STRING, '\0'} // E->c_str
};

///////////////////////////////////////////////////////////////////////////////////
////////// Zasobnik pro zapamatovani operandu  a operace nad zasobnikem ///////////
///////////////////////////////////////////////////////////////////////////////////

// pokud je zasobnik prazdny, last je roven -1
//zasobnik je reprezentovan polem
typedef struct{
  string * array; // pole o velikostli limit
  int last;
  int limit;
}TOperandStack;

///////////////////////// Globalni promena zasobnik ////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
TOperandStack operand_stack; // zasobnik pro zapamatovani hodnot

// inicializace - zasobnik bude globalni
int OperandStackInit(void){
  int i;

  operand_stack.array = (string *) malloc(sizeof(string) * OPERAND_STACK_LIMIT);
  if (operand_stack.array == NULL){
    return INTERNAL_ERROR;
  }
  operand_stack.last = -1;
  operand_stack.limit = OPERAND_STACK_LIMIT;

  for(i = 0; i < OPERAND_STACK_LIMIT; i++){
    operand_stack.array[i] = newString();
    if(operand_stack.array[i] == NULL){
      return INTERNAL_ERROR;
    }
  }

  return OK;
}

// zruseni zasobniku
void disposeOperandStack(void){
  free(operand_stack.array);
}

// funkce pro zvetseni kapacity zasobniku
int reAllocateOperandStack(void){
  string * temp;
  int i;

  temp = (string *) realloc(operand_stack.array, (operand_stack.limit + OPERAND_STACK_LIMIT)*sizeof(string));
  if (temp == NULL){
    disposeOperandStack();
    return INTERNAL_ERROR;
  }

  operand_stack.array = temp;
  i = operand_stack.limit;
  operand_stack.limit = operand_stack.limit + OPERAND_STACK_LIMIT;
  for(i = i; i < operand_stack.limit; i++){
    operand_stack.array[i] = newString();
    if(operand_stack.array[i] == NULL){
      return INTERNAL_ERROR;
    }
  }
  return OK;
}

// funkce pro vlozeni operandu na vrchol zasobniku
// funkce si bude ukladat tokeny ID, a constanty + vygenerovane pom. konstanty
// hodnoty si kopiruje y hodnoty na TLex*
int pushOperand(string temp){
  int ret_value;

  if(operand_stack.last >= operand_stack.limit - 1){
    ret_value = reAllocateOperandStack();
    if(ret_value != OK){
      return ret_value; // zasobnik je jiz zrusenej
    }
  }

  operand_stack.last++;
  // prekopirovani hodnot
  operand_stack.array[operand_stack.last] = newString();
  ret_value = strCopyString((operand_stack.array[operand_stack.last]), temp);
  if(ret_value != OK){
    disposeOperandStack();
    return INTERNAL_ERROR;
  }

  return OK;
}

//funkce pro vybrani operandu z vrcholu
string popOperand(){
  operand_stack.last--;

  return (operand_stack.array[operand_stack.last + 1]);
}


///////////////////////////////////////////////////////////////////////////////////
///////////////////////// Zasobnik a operace nad nim //////////////////////////////
///////////////////////////////////////////////////////////////////////////////////


// datovy typ pro zasobnik
typedef struct stack{
  char *array;  // ukazatel na pole
  int terminal; // index posledniho terminalu
  int last;     // index posledniho prvku
  int limit;    // velikost zasobniku
}TStack;

// inicializace zasobniku pred jeho prvnim pouzitim
// v pripade neuspechu alokace pameti vraci 5
int stackInit(TStack *stack){
  stack->array = (char *) malloc(sizeof(char) * STACK_LIMIT);
  if (stack->array == NULL){
    return INTERNAL_ERROR;
  }
  stack->array[0] = END_OF_LINE;
  stack->terminal = 0;
  stack->last = 0;
  stack->limit = STACK_LIMIT;

  return OK;
}

// zruseni zasobniku
void disposeStack(TStack *stack){
  free(stack->array);
}


// funkce pro zvetseni kapacity zasobniku
int reAllocateStack(TStack *stack){
  char *temp;

  temp = (char *) realloc(stack->array, (stack->limit + STACK_LIMIT)*sizeof(char));
  if (temp == NULL){
    disposeStack(stack);
    return INTERNAL_ERROR;
  }

  stack->array = temp;
  stack->limit = stack->limit + STACK_LIMIT;
  return OK;
}


// funkce pro vlozeni hodnoty do zasobniku
int push(TStack *stack, int value){
  int ret_value;

  if(stack->last >= stack->limit - 1){
    ret_value = reAllocateStack(stack);
    if(ret_value != OK){
      return ret_value; // zasobnik je jiz zrusenej
    }
  }

  stack->last++;
  stack->array[stack->last] = value;
  if(value >= ID && value <= END_OF_FILE){ // kdyz vkladam terminal, tak bude na konci pole
    stack->terminal = stack->last;
  }

  #ifndef SEM_OFF
  /////////////////////////////////////////////////////////////////////////////////
  /////////////////////// jestli je token leva zavorka ... tak se muze jednat o funkci
  ////////////
 // int pommm = stack->array[stack->last - 1];
  if(value == LEFT_BRACKET && stack->array[stack->last - 1] != P){
    tData *temp = NULL;
    int found;
    temp = TableSearch( STableList.Last->table ,operand_stack.array[operand_stack.last], &found);
    if(found){ // nasel jsem zapamatuji si o jakou funkci jde
      if(temp->type == T_FUNCTION)
      { // je to opravdu funkce?
        p_function = &(temp->value.function);
        int a = p_function->declared;
        int b = p_function->defined;
        if(!a && !b) // je funkce definovana?
          return SEM_ERROR;
      }
      else{
        return SEM_ERROR;
      }
    }
    else{
      return SEM_ERROR;
    }
  }

  #endif
  return OK;
}


// funkce, ktera terminal - T  prepise na T< a pushne nacteny token
int pushShift(TStack *stack, int value){
  int ret_value;
  int temp, i, insert = P; // P - paste: < .. ciselna hodnota P = 50

  if(stack->last >= stack->limit - 1){
    ret_value = reAllocateStack(stack);
    if(ret_value != OK){
      return ret_value; // zasobnik je jiz zrusenej
    }
  }

  stack->last++; //pridanim prvku P se nam posune konec o jedno vice
  // za terminal vlozi P a posune zbytek "pole"
  for(i = stack->terminal + 1; i <= stack->last; i++ ){
    temp = stack->array[i];
    stack->array[i] = insert;
    insert = temp;
  }
  // poslednim terminalem bude hodnota value - index nam zapise funkce push
  ret_value = push(stack, token.type);
  if(ret_value != OK){
    return ret_value;
  }

  //////////////////////////////////////////////////////////////////////////////////
  //////////////// jestlize je token "id || const" tak si ho ulozim do zasobniku operandu
  if(value >= ID && value <= CONST_STRING){
    ret_value = pushOperand(token.attribut);
    if(ret_value != OK){
       return ret_value;
    }
  }
  return OK;
}

// funkce pro ziskani indexu P
int indexOfP(TStack *stack){
  int index = stack->last;

  while(stack->array[index] != P){
    index--;
  }

  return index;
}

//funkce pro ziskani indexu terminalu
int indexOfTerminal(TStack *stack){
  int index = stack->last;

  while(stack->array[index] < ID || stack->array[index] > END_OF_LINE){ // ke druhemu by nemelo dojit existuje tam jen expression
    index--;
  }

  return index;
}


// funkce pro redukci
// pokusi se zredukovat vyraz, pri neuspechu vraci chybu
int reduce(TStack *stack, int *rule){
  char temp[50]; // pro redukci budu potrebovat maximalne 4 + 1 na '\0'
  int index_P, i, j = 0, ret_value;
  int find_rule = FALSE;

  index_P = indexOfP(stack); // zjisti na jakem indexu je < nebili P
  for(i = index_P + 1; i <= stack->last; i++) {
    assert(j < 50);
    temp[j] = stack->array[i];  // nakopirovani vyrazu do pomocneho pole
    j++;
  }
  temp[j] = '\0';

  // pokusim se najit pravidlo
  for(i = 0; i < 19; i++){
    ret_value = strcmp(temp, rules[i]);
    if(ret_value == 0){
      find_rule = TRUE;
      break; // nasel jsem vyskoc z cyklu
    }
  }

  if(find_rule == TRUE){
    *rule = i + 1; // pravidlo jak je ocislovane v precedencni tabulce
  }
  else{ // nenasel jsem
    return SYNTAX_ERROR;
  }

  // nasel jsem pravidlo, ulozil jsem si jeho cislo, upravim zasobnik a vlozim EXPRESSION
  stack->last = index_P - 1;
  stack->terminal = indexOfTerminal(stack);
  ret_value = push(stack, EXPRESSION);
  if(ret_value != OK){
    return ret_value;
  }

  return OK;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////// Semanticke akce a generovani kodu ///////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

int semanticActions(int rule){
  tData *temp, *operand1, *operand2, *temp2, *label;
  int found, ret_value, i, p_type;
  string new_variable, new_variable2, c_value;// promena pro nove jmeno nazvu promene
  c_value = newString();
  int function_find = FALSE;
  int function_sort = FALSE;
  int int_to_double = FALSE; // vyuziti jen u konstant

  switch(rule){
    case 1: // E->i()
      if(p_function->params->str[parameter_index +1] != '\0'){ //nesedi pocet parametru
        return SEM_ERROR;
      }

      //
      i = p_function->params->str[0];

      if(i == 'i')
        p_type = T_INT;
      else if(i == 'd')
        p_type = T_DOUBLE;
      else
        p_type = T_STRING;

        // vygenerovani nazvu nove promene
        new_variable = newString();
        if(new_variable == NULL){
          return INTERNAL_ERROR;
        }
        newKey(new_variable);

        temp = TableInsert (&STableList.Act->table, new_variable, p_type);
        if(temp == NULL)
          return INTERNAL_ERROR;

        label = TableSearch (STableList.Last->table, popOperand(), &found);
        if(!found){
          return SEM_ERROR;
        }

        generate(I_ALLOC, temp, NULL, NULL);
        generate(I_CALL, label, NULL, NULL);

        ret_value = pushOperand(new_variable);
        if(ret_value != OK)
          return INTERNAL_ERROR;

      p_function = NULL; // funkci jsem uz vygeneroval a nepotrebuju udaje o ni
      parameter_index = 0;
     break;

    case 2: // E->i(E)
      if(p_function->params->str[parameter_index +1] != '\0'){ //nesedi pocet parametru
        return SEM_ERROR;
      }
      if((strcmp((operand_stack.array[operand_stack.last])->str, "FIND")) == 0){
        function_find = TRUE;
      }
      else if((strcmp((operand_stack.array[operand_stack.last])->str, "SORT")) == 0){
        function_sort = TRUE;
      }
      // vymezit misto pro navratovou hodnotu
      // napushovat parametry
      // zavolat funkci
      i = p_function->params->str[0];

      if(i == 'i')
        p_type = T_INT;
      else if(i == 'd')
        p_type = T_DOUBLE;
      else
        p_type = T_STRING;

        // vygenerovani nazvu nove promene
        new_variable = newString();
        if(new_variable == NULL){
          return INTERNAL_ERROR;
        }
        newKey(new_variable);

        temp = TableInsert (&STableList.Act->table, new_variable, p_type);
        if(temp == NULL)
          return INTERNAL_ERROR;



        generate(I_ALLOC, temp, NULL, NULL);

        /// jestli je funkce vestavena find nebo sort, specialni volani
        if(function_find == TRUE){ //volani funkce find
            operand2 = TableSearch(STableList.Act->table, popOperand(), &found);
            operand1 = TableSearch(STableList.Act->table, popOperand(), &found);
            generate(I_FIND, operand1, operand2, temp);
        }
        else if(function_sort == TRUE){ //volani funkce sort
            operand1 = TableSearch(STableList.Act->table, popOperand(), &found);
            generate(I_SORT, operand1, NULL, temp);
        }
        else
        {
            label = TableSearch (STableList.Last->table, operand_stack.array[operand_stack.last - parameter_index], &found);
            if(!found){
                return SEM_ERROR;
            }

            for(i = operand_stack.last - parameter_index + 1; i <= operand_stack.last; i++)
            {
                temp2 = TableSearch(STableList.Act->table, operand_stack.array[i], &found);
                generate(I_PUSH, temp2, NULL, NULL);
            }
            generate(I_CALL, label, NULL, NULL);


        operand_stack.last = operand_stack.last - parameter_index - 1;
        ret_value = pushOperand(new_variable);
        if(ret_value != OK)
          return INTERNAL_ERROR;
       }


      p_function = NULL; // funkci jsem uz vygeneroval a nepotrebuju udaje o ni
      parameter_index = 0;
     break;

    case 3: // E->E,E
      // nedela nic, parametry kontroluju pri zpracovani na E - pravidla 16 17 18 19
     break;

    case 4: // E->E=E    ve vsech pripadech se nacitaji 2 operandy a vytvari nova promena
    case 5: // E->E<>E   tak to udelam a pak pokracuju
    case 6: // E->E<E
    case 7: // E->E<=E
    case 8: // E->E>E
    case 9: // E->E>=E
    case 10: // E->E+E
    case 11: // E->E-E
    case 12: // E->E*E
    case 13: // E->E/E
    case 14: // E->E\E ..celociselne
        // vygenerovani nazvu nove promene
        new_variable = newString();
        if(new_variable == NULL){
          return INTERNAL_ERROR;
        }
        newKey(new_variable);


        //nacteni druheho operandu
        operand2 = TableSearch(STableList.Act->table, popOperand(), &found);
        if(!found){
          return SEM_ERROR;
        }
        //nacteni prvniho operandu
        operand1 = TableSearch(STableList.Act->table, popOperand(), &found);
        if(!found){
          return SEM_ERROR;
        }

        // vlozeni pomocne promene do TS kde bude vysledek porovnani
        // pozor !!! nutno potom zmenit typ nove promene!!!
        temp = TableInsert (&STableList.Act->table, new_variable, operand1->type);



        if(temp == NULL){
          return INTERNAL_ERROR;
        }
        ret_value = pushOperand(new_variable);
        if(ret_value != OK){
          return INTERNAL_ERROR;
        }
    //dalsi sem akce pro pravidla 4 - 14
        switch(rule){
          case 4: // E->E=E    oba operandy musijou byt stejneho typu
          case 5: // E->E<>E
          case 6: // E->E<E
          case 7: // E->E<=E
          case 8: // E->E>E
          case 9: // E->E>=E
            // kontrola, zda jsou oba operandy stejneho typu
            if(operand1->type != operand2->type){
              return SEM_ERROR;
            }
            temp->type = T_INT;

            //generate vygenerovani instrukce pro vymezeni mista na zasobniku interpretra temp
            //generate samotne vygenerovani instrukce pro jednotlive porovnani
            // alokace pomocne promene
            generate(I_ALLOC, temp, NULL, NULL);
            switch(rule){
              case 4: // E->E=E
               generate(I_TEST_EQ, operand1, operand2, temp);

               break;
              case 5: // E->E<>E
               generate(I_TEST_NEQ, operand1, operand2, temp);
               break;
              case 6: // E->E<E
               generate(I_TEST_L, operand1, operand2, temp);
               break;
              case 7: // E->E<=E
               generate(I_TEST_LEQ, operand1, operand2, temp);
               break;
              case 8: // E->E>E
               generate(I_TEST_G, operand1, operand2, temp);
               break;
              case 9: // E->E>=E
               generate(I_TEST_GEQ, operand1, operand2, temp);
               break;
            }
           break;
          case 10: // E->E+E
          case 11: // E->E-E
          case 12: // E->E*E
            // pokud jsou oba stejny
            if(operand1->type == operand2->type && operand1->type != T_STRING){
              temp->type = operand1->type;
            }
            else if(operand1->type == T_INT && operand2->type == T_DOUBLE){ //nutno pretypovat operand1 na double
               temp->type = T_DOUBLE; // pro ulozeni vysledku
               // nova promena, kam se ulozi cislo prevedene na double
               new_variable2 = newString();
               if(new_variable2 == NULL){
                 return INTERNAL_ERROR;
               }
               newKey(new_variable2);
               temp2 = TableInsert (&STableList.Act->table, new_variable2, T_DOUBLE);
               if(temp2 == NULL){
                 return INTERNAL_ERROR;
               }
               //generate vygenerovani instrukce pro rezervace mista na zasobniku interpretra temp2
               //generate vygenerovani instrukce pro pretypovani operandu 1 na double do temp2
               generate(I_ALLOC, temp2, NULL, NULL);
               generate(I_INT_TO_DOUBLE, operand1, NULL, temp2);

               operand1 = temp2;
            }
            else if(operand2->type == T_INT && operand1->type == T_DOUBLE){ // nutno pretypovat operand 2 na double
               temp->type = T_DOUBLE;
               // nova promena, kam se ulozi cislo prevedene na double
               new_variable2 = newString();
               if(new_variable2 == NULL){
                 return INTERNAL_ERROR;
               }
               newKey(new_variable2);
               temp2 = TableInsert (&STableList.Act->table, new_variable2, T_DOUBLE);
               if(temp2 == NULL){
                 return INTERNAL_ERROR;
               }
               //generate vygenerovani instrukce pro rezervace mista na zasobniku interpretra temp2
               //generate vygenerovani instrukce pro pretypovani operandu 2 na double do temp2
               generate(I_ALLOC, temp2, NULL, NULL);
               generate(I_INT_TO_DOUBLE, operand2, NULL, temp2);

               operand2 = temp2;
            }
            else if(rule == 10 && operand1->type == T_STRING && operand2->type == T_STRING){
              //konkatenace retezcu
              temp->type = T_STRING;
            }
            else{
              return SEM_ERROR;
            }
            //generate vygenerovani instrukce pro vymezeni mista na zasobniku interpretra na temp
            //generate vygenerovani samotne instrukce pro dane operace
            // alokace pomocne promene
            generate(I_ALLOC, temp, NULL, NULL);
            switch(rule){
              case 10: // E->E+E
               generate(I_PLUS, operand1, operand2, temp);
               break;
              case 11: // E->E-E
               generate(I_MINUS, operand1, operand2, temp);
               break;
              case 12: // E->E*E
               generate(I_MULT, operand1, operand2, temp);
               break;
            }

           break;
          case 13: // E->E/E
            // kontrola, zda jsou oba operandy stejneho typu
            if(operand1->type == T_STRING || operand2->type == T_STRING){
              return SEM_ERROR;
            }
            temp->type = T_DOUBLE;


            if(operand1->type == T_INT){
               // nova promena, kam se ulozi cislo prevedene na double
               new_variable = newString();
               if(new_variable == NULL){
                 return INTERNAL_ERROR;
               }
               newKey(new_variable);
               temp2 = TableInsert (&STableList.Act->table, new_variable, T_DOUBLE);
               if(temp2 == NULL){
                 return INTERNAL_ERROR;
               }
               //todo vygenerovani instrukce pro rezervace mista na zasobniku interpretra temp2
               //todo vygenerovani instrukce pro pretypovani operandu 1 na double do temp2
               generate(I_ALLOC, temp2, NULL, NULL);
               generate(I_INT_TO_DOUBLE, operand1, NULL, temp2);

               operand1 = temp2;
            }
            if(operand2->type == T_INT){
               // nova promena, kam se ulozi cislo prevedene na double
               new_variable2 = newString();
               if(new_variable2 == NULL){
                 return INTERNAL_ERROR;
               }
               newKey(new_variable2);
               temp2 = TableInsert (&STableList.Act->table, new_variable2, T_DOUBLE);
               if(temp2 == NULL){
                 return INTERNAL_ERROR;
               }
               //todo vygenerovani instrukce pro rezervace mista na zasobniku interpretra temp2
               //todo vygenerovani instrukce pro pretypovani operandu 2 na double do temp2
               generate(I_ALLOC, temp2, NULL, NULL);
               generate(I_INT_TO_DOUBLE, operand2, NULL, temp2);

               operand2 = temp2;
            }

            //todo vygenerovani instrukce pro vymezeni mista na zasobniku interpretra na temp
            //todo vygenerovani samotne instrukce pro double deleni
            generate(I_ALLOC, temp, NULL, NULL);
            generate(I_DIV, operand1, operand2, temp);

           break;
          case 14: // E->E\E ..celociselne
            if(operand1->type == T_INT && operand2->type == T_INT){
              temp->type = operand1->type;
            }
            else{
              return SEM_ERROR;
            }
            //todo vygenerovani instrukce pro vymezeni mista na zasobniku interpretra na temp
            //todo vygenerovani samotne instrukce pro celociselne deleni
            generate(I_ALLOC, temp, NULL, NULL);
            generate(I_DIV, operand1, operand2, temp);

           break;
        }

     break;

    case 15: // E->(E)
      // neprovadim zadnou akci
     break;

    case 16: // E->i
      // kontrola deklarace promene
      temp = TableSearch(STableList.Act->table, operand_stack.array[operand_stack.last], &found);
      if(!found){
        return SEM_ERROR;
      }
      if(temp->type != T_INT && temp->type != T_DOUBLE && temp->type != T_STRING){
        return SEM_ERROR;
      }

      if(p_function != NULL){ // prisel mi parametr funkce
        int i;
        parameter_index++;

        i = p_function->params->str[parameter_index];
        if(i == '\0'){
          return SEM_ERROR;
        }
        else if(temp->type == T_INT && i == 'd'){ // do funkce byl zadan typ int, ale je pozadovan double
          new_variable = newString();             // automaticka konverze ... musim pretypovat
          if(new_variable == NULL){
            return INTERNAL_ERROR;
           }
           newKey(new_variable);
           temp2 = TableInsert (&STableList.Act->table, new_variable, T_DOUBLE);
           if(temp2 == NULL){
             return INTERNAL_ERROR;
           }
           //todo vygenerovani instrukce pro rezervace mista na zasobniku interpretra temp2
           //todo vygenerovani instrukce pro pretypovani operandu 2 na double do temp2
           generate(I_ALLOC, temp2, NULL, NULL);
           generate(I_INT_TO_DOUBLE, temp, NULL, temp2);

           new_variable2 = popOperand(); // zahodim
           pushOperand(new_variable); // ulozim si tam pretypovanou
        }
        else if(temp->type == T_DOUBLE && i != 'd'){
          return SEM_ERROR;
        }
        else if(temp->type == T_STRING && i != 's'){
          return SEM_ERROR;
        }
        else if(temp->type == T_INT && i != 'i'){
          return SEM_ERROR;
        }
      }
     break;

    case 17: // E->c_int
      if(p_function != NULL){ // prisel mi parametr funkce
        int i;
        parameter_index++;

        i = p_function->params->str[parameter_index];
        if(i == '\0'){
          return SEM_ERROR;
        }
        else if(i == 'd'){ // parametr funkce je double ale ja mam int .. musim pretypovat
          int_to_double = TRUE;
        }
        else if(i != 'i'){
          return SEM_ERROR;
        }
      }

      // vygenerovani nazvu nove promene
      new_variable = newString();
      if(new_variable == NULL){
        return INTERNAL_ERROR;
      }
      newKey(new_variable);
      // vlozeni pomocne promene do TS kde bude vysledek porovnani
      temp = TableInsert (&STableList.Act->table, new_variable, T_INT);
      if(temp == NULL){
        return INTERNAL_ERROR;
      }
      c_value = popOperand();
      ret_value = pushOperand(new_variable);
      if(ret_value != OK){
        return INTERNAL_ERROR;
      }
      //todo inctrukce pro vymezeni mista na zasobniku interpretru
      //todo instrukce pro nahrani do zasobniku z c_value
      generate(I_ALLOC, temp, NULL, NULL);
      generate(I_CONST, temp, c_value, NULL);
// pretypovani kvuli funkci
      if(int_to_double == TRUE){ // pretypovani
           new_variable2 = newString();             // automaticka konverze ... musim pretypovat
           if(new_variable2 == NULL){
            return INTERNAL_ERROR;
           }
           newKey(new_variable2);
           temp2 = TableInsert (&STableList.Act->table, new_variable2, T_DOUBLE);
           if(temp2 == NULL){
             return INTERNAL_ERROR;
           }
           //todo vygenerovani instrukce pro rezervace mista na zasobniku interpretra temp2
           //todo vygenerovani instrukce pro pretypovani operandu 2 na double do temp2
           generate(I_ALLOC, temp2, NULL, NULL);
           generate(I_INT_TO_DOUBLE, temp, NULL, temp2);

           new_variable = popOperand(); // zahodim
           pushOperand(new_variable2); // ulozim si tam pretypovanou
      }



     break;

    case 18: // E->c_d.
      if(p_function != NULL){ // prisel mi parametr funkce
        int i;
        parameter_index++;

        i = p_function->params->str[parameter_index];
        if(i == '\0'){
          return SEM_ERROR;
        }
        else if(i != 'd'){
          return SEM_ERROR;
        }
      }

      // vygenerovani nazvu nove promene
      new_variable = newString();
      if(new_variable == NULL){
        return INTERNAL_ERROR;
      }
      newKey(new_variable);
      // vlozeni pomocne promene do TS kde bude vysledek porovnani
      temp = TableInsert (&STableList.Act->table, new_variable, T_DOUBLE);
      if(temp == NULL){
        return INTERNAL_ERROR;
      }
      c_value = popOperand();
      ret_value = pushOperand(new_variable);
      if(ret_value != OK){
        return INTERNAL_ERROR;
      }
      //todo inctrukce pro vymezeni mista na zasobniku interpretru
      //todo instrukce pro nahrani do zasobniku z c_value
      generate(I_ALLOC, temp, NULL, NULL);
      generate(I_CONST, temp, c_value, NULL);
     break;

    case 19: // E->c_str
      if(p_function != NULL){ // prisel mi parametr funkce
        int i;
        parameter_index++;

        i = p_function->params->str[parameter_index];
        if(i == '\0'){
          return SEM_ERROR;
        }
        else if(i != 's'){
          return SEM_ERROR;
        }
      }

      // vygenerovani nazvu nove promene
      new_variable = newString();
      if(new_variable == NULL){
        return INTERNAL_ERROR;
      }
      newKey(new_variable);
      // vlozeni pomocne promene do TS kde bude vysledek porovnani
      temp = TableInsert (&STableList.Act->table, new_variable, T_STRING);
      if(temp == NULL){
        return INTERNAL_ERROR;
      }
      c_value = popOperand();
      ret_value = pushOperand(new_variable);
      if(ret_value != OK){
        return INTERNAL_ERROR;
      }
      //todo inctrukce pro vymezeni mista na zasobniku interpretru
      //todo instrukce pro nahrani do zasobniku z c_value
      generate(I_ALLOC, temp, NULL, NULL);
      generate(I_CONST, temp, c_value, NULL);
     break;

    default: // zadne jine pravidlo nemuze nastat
      return SEM_ERROR;
     break;
  }

  return OK;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// Funkce pro precedencni analyzu /////////////////////////////////////
//////////////////////////////////// zpristupnena ostatnim modulum  /////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

int expresionParser(tData *result){
  TStack stack;  // zasobnik pro syntaktickou anal.
  int ret_value; // navratova hodnota z funkci
  int action; // urcuje, co se bude dit: redukce, ulozeni na zasobnik ...
  int read_token = FALSE;
  int rule = 0; // jake pravidlo se bude cist
  tData *temp;

  if(OperandStackInit() != OK){ // inicializace zasobniku na operandy
    return INTERNAL_ERROR;
  }

  if(stackInit(&stack) != OK){ // operace nesmi byt provadeny nad neinicializovanym zasobnikem
    disposeOperandStack();
    return INTERNAL_ERROR;
  }

  do{
    //nacteni tokenu
    if(read_token == TRUE){ // jetli je potreba
      ret_value = getNextToken(srcFile, &token); //nacteni tokenu
      if(ret_value != OK){
        disposeStack(&stack);
        disposeOperandStack();
        return ret_value;
      }
      else if(token.type == THEN || token.type == SEMICOLON){
        ;
      }
      else if(token.type == FIND || token.type == SORT){ //////////////////////////////////////////////////////////////////////////////////
        token.type = ID;
      }
      else if(token.type > END_OF_LINE || token.type < ID){ // neni-li v rozsahu ID - END_OF_LINE .. chyba
        disposeStack(&stack);
        disposeOperandStack();
        return SYNTAX_ERROR;
      }
      read_token = FALSE;
    }


    // vyhledani akce, co se bude delat
    if(token.type == THEN || token.type == SEMICOLON){ // byl by problematicke je pridat do precedencni tabulky
      if(stack.array[stack.terminal] == END_OF_LINE){  // proto to zde mam takto
        action = S;
      }
      else{
        action = R;
      }
    }
    else{
      action = prec_table[stack.array[stack.terminal] - ID][token.type - ID]; // ulozeno v enumu ID neni od nuly, proto odecitam
    }

    switch(action){

      case P:
        // za posledni terminal vlozi < a na konec zasobniku vlozi token
        ret_value = pushShift(&stack, token.type);
	    if(ret_value != OK){
	      disposeStack(&stack);
          disposeOperandStack();
	      return ret_value;
	     }
	     read_token = TRUE;
       break;

      case R:
        // redukce az po < a hledani pravidla
        ret_value = reduce(&stack, &rule);
	    if(ret_value != OK){
	      disposeStack(&stack);
          disposeOperandStack();
	      return ret_value;
	    }
        //semanticke akce
        #ifndef SEM_OFF
        ret_value = semanticActions(rule);
        if(ret_value != OK){
          disposeStack(&stack);
          disposeOperandStack();
	      return ret_value;
        }
        #endif
	    read_token = FALSE;
       break;

      case I:
        // pouze ulozi token na vrchol zasobniku
        ret_value = push(&stack, token.type);
	    if(ret_value != OK){
	      disposeStack(&stack);
          disposeOperandStack();
	      return ret_value;
	    }
	    read_token = TRUE;
       break;

      case E:
        // doslo k chybe
//printf(",,,,,error\n");
        disposeStack(&stack);
        disposeOperandStack();
	    return SYNTAX_ERROR;
       break;

      case S:
      //todo prirazeni vysledku do promene: result - od parseru
//printf(",,,,,succes\n");
        temp = TableSearch(STableList.Act->table, popOperand(), NULL);
        result->type = temp->type;
        generate(I_MOVE, temp, NULL, result);

        disposeStack(&stack);
        disposeOperandStack();
        return OK; // precedencni analyza probehla v poradku
       break;

      default:
        disposeStack(&stack); // sem by program nemel nikdz dojit
        disposeOperandStack();
	return INTERNAL_ERROR;
       break;
    }


  } while(action != S);
// sem by se program nemel dostat
  return 0;
}

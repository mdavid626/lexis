
/*****************************************************
** Soubor:    ial.c
** Encoding:  ISO-8859-2
** Datum:     2010/29/11
** Autor:     David Molnár, xmolna02
** Projekt:   IAL, IFJ
** Nazev:     Implementace interpretu imperativniho jazyka IFJ10
** Popis:     Zdrojovy kod vestavenej funkci.
*****************************************************
**/

#include "str.h"
#include "ial.h"

#define NDEBUG

/* Vyhleda zadany podretezec substr v retezci str a vrati jeho pozici 
   (pocitani od nuly). Pokud podretezec neni nalezen, vrati -1.
   Pouzita Boyer-Mooreuv algoritmus.*/
int findStr(string str, string substr)
{
    // Heuristika
    int charJump[ASCII_COUNT];
    
    for (int i = 0; i < ASCII_COUNT; i++)
    {
        charJump[i] = substr->length;
    }
    
    for (int k = 0; k < substr->length; k++)
    {
        int index = substr->str[k];
        charJump[index] = substr->length - k - 1;   
    }
    
    // Hledani
    int j = substr->length - 1;   // index to textu
    int k = substr->length - 1;   // index do vzorku
    
    while (j < str->length && (k >= 0))
    {
        if (str->str[j] == substr->str[k])
        {
            j -= 1;
            k -= 1;    
        }
        else
        {
            int index = str->str[j];
            j += charJump[index];
            k = substr->length - 1;
        }
    } 
    
    if (k < 0)
    {
       return j + 1; // nasla se shoda
    }
    else
    {
       return -1; // shoda se nenasla
    } 
}

/* Znovuustavi hromadu porusenou v koreni - proseje prvek v koreni, ktery 
   jako jediny porusuje pravidlo hromady.
   left je korenovy uzel porusujici pravidla heapu, right je velikost pole*/
void siftDown(string str, int left, int right)
{   
    int done;    // pomocna promenna cyklu
    int maxIndex;
    char temp;   // pomocna promanna
    
    done = 0;
    
    while ((left * 2 <= right) && (done == 0))
    {
        if ((left * 2) == right)
        {
            maxIndex = left * 2;  
        } 
        else if (str->str[left * 2] > str->str[left * 2 + 1]) 
        {
            maxIndex = left * 2;
        } 
        else
        {
            maxIndex = left * 2 + 1;
        }
         
        if (str->str[left] < str->str[maxIndex])
        {
            temp = str->str[left];
            str->str[left] = str->str[maxIndex];
            str->str[maxIndex] = temp;
            left = maxIndex; 
        }
        else
        {
            done = 1;
        }
    }
}

/* Seradi znaky v retezci str tak, aby znak s nizsi ordinalni hodnotou vzdy 
   predchazel pred znakem s vyssi ordinalni hodnotou. 
   Vracen je retezec obsahujici serazene znaky. 
   Pouzita metoda: Heap sort.*/
string sortStr(string str)
{
     string result = newString();
     //strInit(result);
     strCopyString(result, str);
     
     int left = result->length / 2;  // index nejpravejsiho uzlu na nejnizsi urovni
     int right = result->length;
     
     for (int i = left; i >= 0; i--)
     {
        siftDown(result, i, right);
     }
     
     //Vlastni cyklus Heap-sortu
     for (right = result->length - 1; right >= 1; right--)
     {
         char temp = result->str[0];      // Vymena korene s akt. poslednim prvkem
         result->str[0] = result->str[right];
         result->str[right] = temp; 
         
         siftDown(result, 0, right - 1); // Znovuustaveni hromady
     }   
     
     return result;
}

#include "table.h"
#include "assert.h"

extern int offset;

//#define NDEBUG

tItem newTItem(string key, enum data_types type)
{
    tItem node = (tItem) malloc( sizeof(struct item) );
    if(node != NULL )
    {
        node->key = newString();
        strCopyString(node->key, key );
        node->data.type = type;
        
        switch (type)
        {
            case T_INT:
            case T_STRING:
            case T_DOUBLE:
                node->data.value.variable.offset = offset;
                node->data.value.variable.next_input = NULL;
				#ifdef PRINT_INSTRUCTION
				printf("\noffset> %d, %s", offset, key->str);
				#endif
                offset++;
                break;
            case T_UNKNOWN:
                node->data.value.variable.next_input = NULL;
                node->data.value.variable.offset = offset;
				#ifdef PRINT_INSTRUCTION
				printf("\noffset>%d, %s", offset, key->str);
				#endif
                offset++;
                break;
            
            case T_FUNCTION:
                node->data.value.function.defined = FALSE;
                node->data.value.function.declared = FALSE;
                node->data.value.function.label = NULL;
                node->data.value.function.localTable = NULL;
                node->data.value.function.params = newString();
                break;
            case T_LABEL:
                node->data.value.label = NULL;
                break;
        }
        
        node->left = NULL;
        node->right = NULL;
    }
    return node;
}

void TableInit(tSymbolTable* root)
{
    *root = NULL;
}

tData* TableSearch(tSymbolTable root, string key, int* found )
{
    if( root == NULL )
    {
        if( found != NULL ) *found = 0;
        return NULL;
    }
    else
    {
        int cmp = strCmpString(root->key, key);     // pomocna pro vysledek porovnavana
        if (cmp == 0)                               // pokud se nasel klic tak vracim ukazatel na Tdata
        {
            if( found != NULL ) *found = TRUE;
            return &(root->data);
        }
        else if ( root->left == NULL && root->right == NULL)
        {
            if( found != NULL ) *found = FALSE;
            return NULL;                          // pokud se hledany klic nenasel vracim NULL
        }
        else if (cmp > 0)
        {
            return TableSearch( root->right, key, found);      // pokud je hledany klic mensi jak aktualni jdu doleva
        }
        else if (cmp < 0)
        {
            return TableSearch(root->left, key, found);    // pokud je hledany klic vetsi jak aktualni jdu doprava
        }
    }
    
    return NULL;  // sem se TableSearch nedostane
}

tData* TableInsert (tSymbolTable* root, string key, enum data_types type)
{
    assert( root != NULL );     //pocatecny predpoklad (pre testovanie)
    
    tItem pomocna;	                            // pomocna pro alokovani
    
	if (*root == NULL)
    {
        pomocna = newTItem(key, type);      // alokovani
        if (pomocna != NULL)                         // kontrola uspesnosti alokovania
        {
            
            *root = pomocna;                        // vlozeni nove polozky do tabulky symbolu
            
            return &(*root)->data;  // vraceni ukazatele na vlozenou polozu
        }
        //////////////// chybovej kod
        else return NULL; //signalizuje chybu INTERNAL_ERROR
    }

	else if (strCmpString( (*root)->key, key) < 0)     // pokud je hledany klic mensi jak aktualni jdu doleva
    {
        return TableInsert(&(*root)->left, key, type);
    }
	else if (strCmpString( (*root)->key,key) > 0)     // pokud je hledany klic vetsi jak aktualni jdu doprava
    {
        return TableInsert( &(*root)->right, key, type);
    }
    else 
    {
        (*root)->data.type = type;     // prepsani hodnoty daneho klice
        return &(*root)->data;
    }
    
    return NULL; //by nemnel nastat

}

void deleteTItem(tItem* item)
{
    assert(item != NULL);
    
//    if( *item != NULL)
//    {
//        if( (*item)->data.type == T_FUNCTION )
//        {
//            strFree((*item)->data.value.function.params);
//            //strClear((*item)->data.value.function.params);
//        }
//        
//        strFree((*item)->key);
//    }
    
    free(*item);
    *item = NULL;
    
}

void TableDispose(tSymbolTable* root)
{
    assert(root != NULL);
    
	if (*root != NULL)
    {
        TableDispose(&(*root)->left);
        TableDispose(&(*root)->right);
        deleteTItem(root);
    }
}

/* konec ial.c */

#include <stdio.h>
#include <stdlib.h>
#include "strbuf.h"
 //#include "arraylist.h"
int main()
{
    //int data[] = {1, 2, 3, 4, 5};
    //strbuf_t* list=malloc(sizeof(strbuf_t));
    strbuf_t* L=malloc(sizeof(strbuf_t));
    //list->data = data;
    int flag;
    //al_init(L, 8);
    flag=sb_init(L, 20);
    if(flag!=1)
    {
    // for(int i=0; i<5; i++)
    // {
    //     sb_append(L, 'q');
    //     sb_append(L, 'w');
    //     sb_append(L, 'o');
    //     // sb_append(L, (char)i);
    //     // sb_append(L, (char)i);
    //     // sb_append(L, (char)i);
    // }
    char str[] = "this is a sentence";
    //printf("the sentence is %s\n", str);
    sb_concat(L, str);
    // sb_remove(L, &L->data[0]);
    //  sb_remove(L, &L->data[1]);
//    sb_insert(L, 15, '5');
//      sb_insert(L,2, 'd' );
    // al_insert(L, 64, -30);

   
        for(int i=0; i<L->length; i++)
        {
            printf("%c", L->data[i]);
        }
        printf("\n");
        
        printf("The length of the array is %lu and the data used is %lu\n", L->length, L->used);
         sb_destroy(L);    
    }
    free(L);

    // al_destroy(L);
    return 0;
}
#include <stdlib.h>
#include <stdio.h>
#include "strbuf.h"

#ifndef DEBUG
#define DEBUG 0
#endif

int sb_init(strbuf_t *L, size_t length)
{
    if(length<1)
    {
        return 1;
    }
    L->data = malloc(sizeof(char) * length);
    if (!L->data) return 1;

    L->length = length;
    L->used   = 1;
    L->data[0]='\0';

    return 0;
}

void sb_destroy(strbuf_t *L)
{
    free(L->data);
}


int sb_append(strbuf_t *L, char item)
{
    if (L->used == L->length) {
	size_t size = L->length * 2;
	char *p = realloc(L->data, sizeof(char) * size);
    if (!p) return 1;

	L->data = p;
	L->length = size;

	if (DEBUG) printf("Increased size to %lu\n", size);
    }
    // if(L->used==0)
    // {
    //     L->used=2;
    //     L->data[0]=item;
    //     L->data[1]='\0';
    // }
        L->data[L->used-1] = item;
        L->data[L->used]='\0';
        L->used++;

    return 0;
}


int sb_remove(strbuf_t *L, char *item)
{
    if (L->used == 0) return 1;

    --L->used;

    if (item) 
    {
        *item = L->data[L->used-1];
        L->data[L->used-1]=L->data[L->used];
    }

    return 0;
}

int sb_insert(strbuf_t *list, int index, char item)
{
    if(index<0)
    {
        return 1;
    }
    if(list->length<=index+1)
    {
        int new_length = (list->length)*2;
        if ((list->length)*2<index+2)
        {
            new_length=index+2;
        }
        list->length=new_length;
        list->data=realloc(list->data, sizeof(char)*new_length);
        if(!list->data)
        {
            return 1;
        }
        list->used=index+2;
    }
    else
    {
        if(list->used<=index)
        {
            list->used = index+2;
        }
        else
        {
            if(list->used==list->length)
            {
                list->length=list->length*2;
                list->data=realloc(list->data, sizeof(char)*list->length);
            }
            list->used++;
            for(int i=list->used-1; i>index; i--)
            {
                list->data[i]=list->data[i-1];
            }
        }
    }

    list->data[index]=item;
    list->data[list->used-1]='\0';
    return 0;
}
int sb_concat(strbuf_t *sb, char *str)
{
    while(*str!='\0')
    {
        //printf("char is %c\n", *str);
        sb_append(sb, *str);
        str++;
    }
    return 0;
}
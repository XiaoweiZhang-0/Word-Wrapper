#include <stdlib.h>
#include <stdio.h>

void triangle(unsigned width)
{
    unsigned i, j;
    
    //i = 1;
    for(i=1; i<=width; i++) {
        //j = 0;
        for(j=0; j<i; j++) {
            putchar('*');
            //++j;
        }
        // printf("   j is %d", j);
        putchar('\n');
    }
    // printf("i is %d", i);
}
void v_triangle(unsigned width)
{
    unsigned i, j;
    for(i=width; i>=1; i--)
    {
        for(j=0; j<i; j++)
        {
            putchar('*');
        }
        // printf("   j is %d", j);
        putchar('\n');
        
    }
}
void h_triangle(unsigned width)
{
    unsigned i, j;
    for(i=width; i>=1; i--)
    {
        for(int l=0; l<width-i; l++)
        {
            printf(" ");
        }
        for(j=0; j<i; j++)
        {
            putchar('*');
        }
        // printf("   j is %d", j);
        putchar('\n');
        
    }
}
int main(int argc, char* argv[])
{
   //printf("%d\n", argc);
   if(argc>=2 && atoi(argv[1]))
   {
       int num = atoi(argv[1]);
    //    char q = *argv[2];
    //    printf("%c\n", q);
    if(argc==3 && *argv[2]=='h')
    {
        h_triangle(num);
    }
    else if(argc==3 && *argv[2]=='v')
    {
        v_triangle(num);
    }
    else if(argc==2)
    {
        triangle(num);
    }
    else
    {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
   }
   else{return EXIT_FAILURE; }

    
    
    
}
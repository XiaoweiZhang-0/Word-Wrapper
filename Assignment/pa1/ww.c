#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

#include <stdio.h>

#define char_length 20
int open_file(int argc, char** argv);
void read_write(int fd, long int page_wdith);
//void print_file(long int, char*);
int main(int argc, char* argv[])
{
    int fd;
    long int page_width;
    fd = open_file(argc, argv);
    page_width = atoi(argv[1]);


    // char buffer;
    // read(fd, &buffer, 1);
    // write(1, &buffer, 1);
    
    read_write(fd, page_width);
    close(fd);

}

//check the number of arguments and open the file
//output: file descriptor number
int open_file(int argc, char** argv)
{
    int fd=0;
    if(argc < 2 || argc > 3)
    {
        char output[] = "Please enter exactly one or two arguments\n";
        write(1, output, strlen(output));
        exit(EXIT_FAILURE);
    }
    assert(argc == 2 || argc == 3);
    if(argc == 3)
    {
        fd = open(argv[2], O_RDONLY);
        if(fd < 0)
        {
            char output[] = "Unable to open the file\n";
            write(1, output, strlen(output));
            exit(EXIT_FAILURE);
        }
    }
    return fd;
}

void read_write(int fd, long int page_width)
{
    char* word = malloc(sizeof(char));
    char buffer[char_length];
    int bytes_read;
    int size_of_word = 0;
    int char_printed = 0;
    //int space_counter = 0;
    char space = ' ';
    char newline = '\n';
    int j = 0;

    char blank_line[page_width+1];
    for(int i=0; i<page_width-1; i++)
    {
        blank_line[i] = ' ';
    }
    blank_line[page_width] = '\n';

    while((bytes_read = read(fd, buffer, char_length)) > 0)
    {
        for(int i=0; i<char_length; i++)
        {
            if(!isspace(buffer[i]))
            {
                size_of_word++;
                word = realloc(word, size_of_word*sizeof(char));
                word[j] = buffer[i];
                j++;
                printf("read another char %c\n", word[j-1]);
            }
            else if(size_of_word != 0)
            {
                //printf("read a space and printing the word, size of word is %d\n", size_of_word);

                if((size_of_word+char_printed) < page_width)
                {
                    write(1, word, size_of_word*sizeof(char));
                    write(1, &space, sizeof(char));
                    char_printed = char_printed + size_of_word + 1;
                    size_of_word = 0; 
                    j = 0;  
                }
                else if((size_of_word+char_printed) == page_width)
                {
                    write(1, word, size_of_word*sizeof(char));
                    write(1, &newline, sizeof(char));
                    char_printed = 0;
                    size_of_word = 0;
                    j = 0;
                    printf("newline\n");
                }
                else
                {
                    write(1, &newline, sizeof(char));
                    write(1, word, size_of_word*sizeof(char));
                    write(1, &space, sizeof(char));
                    char_printed = size_of_word+1;
                    size_of_word = 0;
                    j = 0;
                }
            }
            else
            {   
                
                if(char_printed == 0 && buffer[i] == '\n')
                {
                    printf("read a space\n");
                    write(1, blank_line, page_width+1);
                }
            }
        }
    }
}

// void print_file(long int page_width, char* paragraph)
// {
//     int i=0;
//     char buffer = paragraph[i];
//     char line[page_width];
//     while(buffer != EOF)
//     {
//         //i = i+page_width-1;
//         // while(paragraph[i] != ' ' || paragraph[i] != '\n')
//         // {
//         //     i--;
//         // }
//         //reduce blanks to one between words
//         if(buffer == ' ')
//         {
//             i++;
            
//         }
//         int j=0;
//         for(j; j<page_width; j++)
//         {
//             if()
//         }
//         i = i+j;
//         buffer = paragraph[i];

//     }
// }
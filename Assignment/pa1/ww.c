#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>

#define char_length 1
int open_file(int argc, char** argv);
bool read_write(int fd, long int page_wdith, int output);
bool print_file(int* size_of_word, int* char_printed, long int page_width, char* word, int output);
int main(int argc, char* argv[])
{
    int fd;
    long int page_width;
    bool overlimit;
    int output = 1; // output is the file descriptor where the formatted words should be printed to, 1 means stdout
    fd = open_file(argc, argv);
    page_width = atoi(argv[1]);
    if(page_width <= 0)
    {
        perror("page width has to be greater than 0\n");
        return EXIT_FAILURE;
    }

    overlimit = read_write(fd, page_width, output);
    close(fd);
    if(overlimit)
    {
        return EXIT_FAILURE;
    }
    else
    {
        return EXIT_SUCCESS;
    }
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

//read from file and write to standard output
bool read_write(int fd, long int page_width, int output)
{
    char* word = malloc(sizeof(char)); //word 
    char buffer[char_length]; //buffer for read
    int bytes_read; //store how many bytes have read
    int size_of_word = 0;
    int char_printed = 0; //store the number of char printed on the current line
    int newline_counter = 0; 
    char newline = '\n';
    bool overlimit = false;// flag when a word is overlimit of width
    while((bytes_read = read(fd, buffer, char_length)) >0)
    {
        for(int i=0; i<bytes_read; i++)
        {
            //printf("current char is %c\n", buffer[i]);
            if(!isspace(buffer[i]))
            {
                //printf("j is %d\n", j);
                if(newline_counter > 1)
                {
                    write(output, &newline, 1);
                    write(output, &newline, 1);
                    char_printed = 0;
                }
                newline_counter = 0;
                size_of_word++;
                word = realloc(word, size_of_word*sizeof(char));
                word[size_of_word-1] = buffer[i];
                //printf("read another char %c\n", word[0]);
            }
            else if(size_of_word != 0)
            {
                if(buffer[i] == '\n')
                {
                    newline_counter++;
                }
                //printf("read a space and printing the word, size of word is %d\n", size_of_word);
                overlimit = (overlimit | print_file(&size_of_word, &char_printed, page_width, word, output));
            }
            else if(buffer[i] == '\n')
            {   
                //printf("-----------------------------------read a newline\n");
                newline_counter++;
            }
        }
    }
    //free(buffer);
    if(size_of_word != 0)
    {
        //printf("read a space and printing the word, size of word is %d\n", size_of_word);
        overlimit = (overlimit | print_file(&size_of_word, &char_printed, page_width, word, output));
    }
    write(output, &newline, 1);
    free(word);
    return overlimit;
}

//print the file word by word 
bool print_file(int* size_of_word, int* char_printed, long int page_width, char* word, int output)
{
    char space = ' ';
    char newline = '\n';
    int line_size = (*size_of_word) + (*char_printed);
    bool overlimit = false;
    //printf("word is %c\n", word[0]);
    if(line_size < page_width)
    {
        write(output, word, *size_of_word);
        write(output, &space, sizeof(char));
        *char_printed = *char_printed + *size_of_word + 1;
        *size_of_word = 0; 
        //printf("---a line is populared");  
    }
    else if(line_size == page_width)
    {
        write(output, word, (*size_of_word)*sizeof(char));
        write(output, &newline, sizeof(char));
        *char_printed = 0;
        *size_of_word = 0;
        //printf("---a line is is full");
    }
    else
    {
        if(*size_of_word > page_width)
        {
            overlimit = true;
        }
        if(*char_printed != 0)
        {
            write(output, &newline, sizeof(char));
        }
        write(output, word, (*size_of_word)*sizeof(char));
        write(output, &space, sizeof(char));
        *char_printed = *size_of_word+1;
        *size_of_word = 0;
    }
    return overlimit;
}
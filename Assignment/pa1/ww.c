#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#define char_length 1

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
    
    // read_file(fd, paragraph);
    // print_file(page_width, paragraph);
    close(fd);

}

//check the number of arguments and open the file
//output: file descriptor number
int open_file(int argc, char** argv)
{
    int fd=0;
    // if(argc < 2 || argc > 3)
    // {
    //     char output[] = "Please enter exactly one or two arguments\n";
    //     write(1, output, strlen(output));
    //     exit(EXIT_FAILURE);
    // }
    assert(argc == 2 || argc == 3);
    if(argc == 3)
    {
        fd = open(argv[2], O_RDONLY);
        // if(fd < 0)
        // {
        //     char output[] = "Unable to open the file\n";
        //     write(1, output, strlen(output));
        //     exit(EXIT_FAILURE);
        // }
        assert(fd > 0);
    }
    return fd;
}

// void read_write(int fd, long int page_width)
// {
//     char* word = NULL;
//     char buffer[char_length];
//     int bytes_read;
//     while(bytes_read = read(fd, buffer, char_length) > 0)
//     {
        
//     }
//     paragraph = calloc(j+1, sizeof(char));
//     paragraph[j] = EOF; 
// }

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
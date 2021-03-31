#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <sys/stat.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#ifndef WWPATH
#define WWPATH "./ww"
#endif
int main (int argc, char* argv[])
{
    assert(argc > 2);
    int number_of_file = argc-2;
    int page_width = atoi(argv[1]);
    int exit_code = EXIT_SUCCESS;
    assert(page_width > 0);
    bool count_file = false;
    //printf("number of files is %d\n", argc-2);
    
    for(int i=0; i<number_of_file; i++)
    {
        struct stat buf;
        char* filename = argv[i+2];
        stat(filename, &buf);
        if(S_ISDIR(buf.st_mode))
        {
            //perror("file cannot be a directory");
            fprintf(stderr, "%s","file cannot be a directory");
            exit_code = EXIT_FAILURE;
            continue;
        }
        // printf("file name is %s\n", filename);
        // printf("page_width is %d\n", page_width);
        int fd[2];
        pipe(fd);
        pid_t child = fork();
        if(child < 0)
        {
            fprintf(stderr, "%s","fork failed");
            abort();
        }
        else if(child == 0)
        {
            //child process
            close(fd[0]);
            dup2(fd[1], 1);
            int error = execl(WWPATH, WWPATH, argv[1], filename,(char*) NULL);
            // return EXIT_SUCCESS;
            assert(error != -1);
            fprintf(stderr, "%s","execl failed");
            abort();
        }
        else
        {
            //parent process
            close(fd[1]);
            char buffer[page_width];
            bool nonblank = false;
            int byte_read = read(fd[0], buffer, sizeof(buffer));
            //printf("%s", buffer);
            for(int j=0; j<byte_read; j++)
            {
                if(!isspace(buffer[j]))
                {
                    if(count_file)
                    {
                        printf("\n");
                    }
                    count_file = true;
                    j = page_width+1;
                    nonblank = true;
                }
            }
            if(nonblank)
            {
                printf("%s", buffer);
                memset(buffer, 0, sizeof(buffer));
                while((byte_read = read(fd[0], buffer, sizeof(buffer))) > 0)
                {
                    printf("%s", buffer);
                    memset(buffer, 0, sizeof(buffer));
                }
                
            }
            int wstatus;
            wait(&wstatus);
            close(fd[0]);
            exit_code = (exit_code || WEXITSTATUS(wstatus));
        }
    }

    //printf("exit status is %d\n", exit_code);
    return exit_code;
}
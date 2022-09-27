#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>

#define char_length 1
bool read_write(int fd_r, int fd_w, long int page_width);
bool print_file(int fd, int* size_of_word, int* char_printed, long int page_width, char* word);
bool handle_dir(long int page_width, char * dirpath);
bool handle_file(long int page_width, char * dirpath, char * filename);

int main(int argc, char* argv[]) {
    int rc;
    bool overlimit;
    long int page_width;
    struct stat buf;
    // param check
    if(argc < 2 || argc > 3) {
        char output[] = "Please enter exactly one or two arguments\n";
        write(STDOUT_FILENO, output, strlen(output));
        exit(EXIT_FAILURE);
    }
    assert(argc == 2 || argc == 3);

    page_width = atoi(argv[1]);
    if(page_width <= 0) {
        perror("page width has to be greater than 0");
        return EXIT_FAILURE;
    }

    if (argc == 3) {
        rc = stat(argv[2], &buf);
        if (rc) {
            perror("read file error");
            return EXIT_FAILURE;
        }
        if (S_ISREG(buf.st_mode)) {
            overlimit = handle_file(page_width, "", argv[2]);
        } else if (S_ISDIR(buf.st_mode)) {
            overlimit = handle_dir(page_width,argv[2]);
        } else {
            perror("file is not normal file or dir");
            return EXIT_FAILURE;
        }
    } else {
        overlimit = read_write(STDIN_FILENO, STDOUT_FILENO, page_width);
    }
    if (overlimit) {
        return EXIT_FAILURE;
    } else {
        return EXIT_SUCCESS;
    }
}


bool read_write(int fd_r, int fd_w, long int page_width)
{
    char* word = malloc(sizeof(char)); 
    char buffer[char_length]; 
    int bytes_read; 
    int size_of_word = 0;
    int char_printed = 0; 
    int newline_counter = 0;
    char newline = '\n';
    bool overlimit = false;
    while((bytes_read = read(fd_r, buffer, char_length)) >0)
    {
        for(int i=0; i<bytes_read; i++)
        {
            if(!isspace(buffer[i]))
            {
                if(newline_counter > 1)
                {
                    write(fd_w, &newline, 1);
                    write(fd_w, &newline, 1);
                    char_printed = 0;
                }
                newline_counter = 0;
                size_of_word++;
                word = realloc(word, size_of_word*sizeof(char));
                word[size_of_word-1] = buffer[i];
            }
            else if(size_of_word != 0)
            {
                if(buffer[i] == '\n')
                {
                    newline_counter++;
                }
                overlimit = (overlimit | print_file(fd_w,&size_of_word, &char_printed, page_width, word));
            }
            else if(buffer[i] == '\n')
            {
                newline_counter++;
            }
        }
    }
    if(size_of_word != 0)
    {
        overlimit = (overlimit | print_file(fd_w,&size_of_word, &char_printed, page_width, word));
    }
    write(fd_w, &newline, 1);
    free(word);
    return overlimit;
}

//print the file word by word
bool print_file(int fd, int* size_of_word, int* char_printed, long int page_width, char* word)
{
    char space = ' ';
    char newline = '\n';
    int line_size = (*size_of_word) + (*char_printed) + 1;
    bool overlimit = false;

    if(*char_printed == 0)
    {

        write(fd, word, *size_of_word);
        *char_printed = *size_of_word;
        if(*size_of_word > page_width)
        {
            overlimit = true;
            write(fd, &newline, sizeof(char));
            *char_printed = 0;
        }
        *size_of_word = 0;

    }
    else if(line_size < page_width)
    {
        write(fd, &space, sizeof(char));
        write(fd, word, *size_of_word);
        *char_printed = *char_printed + *size_of_word + 1;
        *size_of_word = 0; 
    }
    else if(line_size == page_width)
    {
        write(fd, &space, sizeof(char));
        write(fd, word, (*size_of_word)*sizeof(char));
        write(fd, &newline, sizeof(char));
        *char_printed = 0;
        *size_of_word = 0;
    }
    else
    {
        if(*size_of_word > page_width)
        {
            overlimit = true;
        }
 
        write(fd, &newline, sizeof(char));
        write(fd, word, (*size_of_word)*sizeof(char));
        *char_printed = *size_of_word;
        *size_of_word = 0;
    }
    return overlimit;
}

bool handle_dir(long int page_width, char * dirpath) {
    DIR *dp;
    struct dirent *dirp;
    char wrap_prefix[]="wrap.";
    char tmp_filepath[BUFSIZ];
    bool overlimit = false;
    dp = opendir(dirpath);
    if (!dp) {
        printf("open dir fail!\n");
        return -1;
    }

    for (dirp = readdir(dp); dirp != NULL ;dirp = readdir(dp)) {
        if (dirp->d_name[0] == '.') {
            continue;
        }
        if (strlen(dirp->d_name) >= strlen(wrap_prefix)) {
            // length of "wrap." is 5
            if (memcmp(dirp->d_name, wrap_prefix, strlen(wrap_prefix)) == 0) {
                continue;
            }
        }
        if (dirp->d_type == 4) {
            // DT_DIR == 4
            sprintf(tmp_filepath, "%s/%s", dirpath, dirp->d_name);
            overlimit = (overlimit | handle_dir(page_width, tmp_filepath));
        } else if (dirp->d_type == 8 ) {
            // DT_REG == 8
            overlimit = (overlimit | handle_file(page_width, dirpath, dirp->d_name));
        } else {
            // bad file type, dismiss
            continue;
        }
    }
    closedir(dp);
    return overlimit;
}

bool handle_file(long int page_width, char * dirpath, char * filename) {
    int fd_r,fd_w;
    char tmp_filepath[BUFSIZ];
    bool overlimit = false;

    if (strlen(dirpath) != 0) {
        sprintf(tmp_filepath, "%s/%s", dirpath, filename);

        fd_r = open(tmp_filepath, O_RDONLY);
        if(fd_r < 0) {
            char output[] = "Unable to open the file for read\n";
            write(STDOUT_FILENO, output, strlen(output));
            return -1;
        }

        sprintf(tmp_filepath, "%s/wrap.%s", dirpath, filename);
        fd_w = open(tmp_filepath, O_WRONLY | O_CREAT | O_TRUNC, 0666);
        if(fd_w < 0) {
            char output[] = "Unable to open the file for write\n";
            write(STDOUT_FILENO, output, strlen(output));
            close(fd_r);
            return -1;
        }

        overlimit = (overlimit | read_write(fd_r, fd_w, page_width));
        close(fd_r);
        close(fd_w);
    } else {
        fd_r = open(filename, O_RDONLY);
        if(fd_r < 0) {
            char output[] = "Unable to open the file for read\n";
            perror(output);
            exit(EXIT_FAILURE);
        }

        overlimit = (overlimit | read_write(fd_r, STDOUT_FILENO, page_width));
        close(fd_r);
    }
    return overlimit;
}

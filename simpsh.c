// simpsh.c

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char** argv)
{
    int i;
    int fileIndex = 0;
    int FILE_CAPACITY = 10;

    int* files;

    files = malloc(FILE_CAPACITY * sizeof(int));

    for (i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "--rdonly") == 0)
        {
            if (argc - 1 < i + 1)
            {
                printf("Error: Missing file\n");
                exit(1);
            }

            int fd = open(argv[i+1], O_RDONLY);
            if (fd == -1)
            {
                printf("Failed to open file\n");
                exit(1);
            }

            if (fileIndex == FILE_CAPACITY)
            {
                FILE_CAPACITY = FILE_CAPACITY * 2;
                files = realloc(files, FILE_CAPACITY * sizeof(int));
                // check maybe
            }

            files[fileIndex] = fd;
            fileIndex++;
        }

        if (strcmp(argv[i], "--wronly") == 0)
        {
            
        }

        if (strcmp(argv[i], "--command") == 0)
        {
            
        }

        if (strcmp(argv[i], "--verbose") == 0)
        {
            
        }
    }


    // close all the ish
    for (i = 0; i < FILE_CAPACITY; i++)
    {
        
    }

    return 0;
}
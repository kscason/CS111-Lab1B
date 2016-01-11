#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <errno.h>
#include  <sys/types.h>

/* Flag set by ‘--verbose’. */
static int verbose_flag;

/* Data/storage for file descriptors */
static int fileIndex;
static int* files;
static int FILE_CAPACITY;

/* Storage for command arguments */
static int CMD_CAPACITY;

/* Storage for I/O/E */
typedef struct 
{
	int fd1, fd2, fd3;
} cmdfds;

/* STD IN/OUT/ERR */
int saved_IN;
int saved_OUT;
int saved_ERR;

void checkmemory()
{
  /* Check memory of allocated file array, 
      add more if at FILE_CAPACITY, 
      exit if allocation error */
  if( fileIndex == FILE_CAPACITY )
  {
    FILE_CAPACITY = FILE_CAPACITY * 2;
    files = realloc(files, FILE_CAPACITY * sizeof(int));
  }
  if( errno == ENOMEM )
    exit(EXIT_FAILURE);
}

void openfile( const char *path, int flag )
{
  /* Open requested file with correct OFLAG */
  int fd = open(path, flag);
  if (fd == -1)
  {
    /* Return error if failure to open file */
    fprintf(stderr, "Error: Failed to open file!\n");
    exit(EXIT_FAILURE);
  }
  printf("File descriptor old: %d && fileIndex: %d\n", fd, fileIndex);
  files[fileIndex] = fd;
  fileIndex++;
}

void runCommand(cmdfds fds, char *args[])
{
  pid_t pid = fork();
	
  if( pid < 0 )
    fprintf( stderr, "Error: Couldn't create a child process!\n" );
  else if (pid == 0)
  {
    /* Child thread to run the bash command */

    /* Change standard input, output, error to user specifications 
        and save current standards for later */
    saved_IN = dup(0);
    saved_OUT = dup(1);
    saved_ERR = dup(2);
    printf( "%d,%d,%d\n", files[fds.fd1],files[fds.fd2],files[fds.fd3]);
    dup2( files[fds.fd1], 0 );
    dup2( files[fds.fd2], 1 );
    dup2( files[fds.fd3], 2 );

    /* By convention, args[0] is the cmd name, args must end with a null ptr */
    dprintf( saved_ERR, "Got to childrens\n"); 
    if(execvp(args[0], args) == -1)
    {
      dprintf( saved_ERR, "Error: Unable to execute command!\n");
      exit(EXIT_FAILURE);
    }
  }
    else
    {
      /* Parent thread to continue processes:
        Restores standard input, output, and error. */
        int returnStatus;    
        waitpid(pid, &returnStatus, 0);
      dup2( saved_IN, 0 );
      dup2( saved_OUT, 1 );
      dup2( saved_ERR, 2 );
      close( saved_IN );
      close( saved_OUT );
      close( saved_ERR );
      printf( "%d,%d,%d\n", files[fds.fd1],files[fds.fd2],files[fds.fd3]);
  }
}

int main (int argc, char **argv)
{
  int c;
  verbose_flag = 0;

  /* Set up array of file descriptors */
  fileIndex = 0;
  FILE_CAPACITY = 10;
  files = malloc(FILE_CAPACITY * sizeof(int));
  checkmemory();
  
  while (1)
    {
      static struct option long_options[] =
        {
          /* These option sets a flag. */
          {"verbose",    no_argument,       &verbose_flag,   1},
          /* These options don’t set a flag.
             We distinguish them by their indices. */
          {"rdonly",     required_argument, 0,              'r'},
          {"wronly",     required_argument, 0,              'w'},
          {"command",    required_argument, 0,              'c'},
          {0,            0,                 0,               0}
        };

      /* getopt_long stores the option index here. */
      int option_index = 0;
      
      /* Empty string "" since no short options allowed. */
      c = getopt_long (argc, argv, "",
                       long_options, &option_index);

      /* Detect the end of the options. */
      if (c == -1)
        break;

      switch (c)
        {
	       case 0:
            /* This option set a flag, do nothing else now. */
                break;

            case 'r':
	           if(verbose_flag)
	               printf ("--rdonly %s\n", optarg);
	  
                checkmemory();
                openfile(optarg, O_RDONLY);
                printf( "%d", optind );
                break;

            case 'w':
                if(verbose_flag)
                    printf ("--wronly %s\n", optarg);
                checkmemory();
                openfile(optarg, O_WRONLY);
                break;

            case 'c':
	           printf( "%d", optind );
	           if( optind+2 > argc )
	           {
	               fprintf( stderr, "Error: Missing additional operands!\n" );
	               exit(EXIT_FAILURE);
	           }
	           if(verbose_flag)
	               printf ("--command %s %s %s %s\n", optarg, argv[optind],
		              argv[optind+1], argv[optind+2]);

    /* Gather file descriptors for i o e */
	  cmdfds stdioe = {atoi(optarg), atoi(argv[optind]),
				  atoi(argv[optind+1])};
	  if( (stdioe.fd1 >= fileIndex) || (stdioe.fd2 >= fileIndex) || 
	      (stdioe.fd3 >= fileIndex) )
    {
	    fprintf( stderr, "Error: File descriptors out of range!\n" );
	    exit(EXIT_FAILURE);
	  }
  printf( "%d,%d,%d\n", files[stdioe.fd1],files[stdioe.fd2],files[stdioe.fd3]);

    /* Collect arguments for the command until another -- is hit.*/
    CMD_CAPACITY = 10;
    char **args = (char**)malloc(CMD_CAPACITY * sizeof(char*));
    /*for( int j = 0; j < CMD_CAPACITY; j++ )
    {
      args[j] = malloc(CMD_CAPACITY * sizeof(char*));
    }*/
    int index = optind + 2;
    int argsCounter = 0;
    while( index <= argc )
    {
      /* If found an argument with leading '--', stop collecting */
      if( index == argc || (argv[index][0] == '-' && argv[index][1] == '-'))
      {
	args[argsCounter] = NULL;
	optind = index;
        break;
      }
      if( argsCounter == CMD_CAPACITY )
      {
        /* Reallocate memory, check me on this. */
        CMD_CAPACITY *= 2;
        args = (char**)realloc(args, CMD_CAPACITY * sizeof(char*));
      }
      args[argsCounter] = argv[index];
      argsCounter++;
      index++;
    }
    //    optind = index;
    //for( int j = 0; j < argsCounter; j++ )
    //printf( "%s ", args[j]);

	  runCommand(stdioe, args);
	  free(args);
	  break;

        case '?':
          /* getopt_long already printed an error message. */
        default:
          exit(EXIT_FAILURE);
        }
    }

  /* Close file descriptors and free allocated memory */
  for( int i = fileIndex-1; i >= 0; i-- )
    close(files[i]);
  free(files);

  // please fix this adam idk how to free memory tbh

  exit (EXIT_SUCCESS);
}

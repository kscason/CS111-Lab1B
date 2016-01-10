#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <errno.h> //check how else you wanna check reallocs lol
#include  <sys/types.h>


/* Flag set by ‘--verbose’. */
static int verbose_flag;

/* Data/storage for file descriptors */
static int fileIndex;
static int* files;
static int FILE_CAPACITY;

/* Storage for command arguments */
static char* commandArgs;

/*<<<<<<< HEAD
/* Storage for I/O/E 
struct cmdfds{
  int fd1;
  int fd2;
  int fd3;
}; 
=======
*/
typedef struct {
	int fd1, fd2, fd3;
} cmdfds;

//>>>>>>> c8c7bde80e5d7e88476de1472eabeef227352360

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

void runCommand(cmdfds fds, const char* cmd, const char** args)
{
  int pid = fork();
  if( pid < 0 )
    fprintf( stderr, "Error: Couldn't create a child process!" );
  else if (pid == 0)
  {	
    printf("Child thread\n");
    exit(EXIT_SUCCESS);
  }
	else
	{
		printf("Parent thread\n");
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
	  break;

        case 'w':
	  if(verbose_flag)
	    printf ("--wronly %s\n", optarg);
	  checkmemory();
	  openfile(optarg, O_WRONLY);
	  break;

        case 'c':
	  if( optind+2 > argc )
	  {
	    fprintf( stderr, "Error: Missing additional operands!\n" );
	    exit(EXIT_FAILURE);
	  }
	  if(verbose_flag)
	    printf ("--command %s %s %s %s\n", optarg, argv[optind],
		    argv[optind+1], argv[optind+2]);
	  //gather stdin, stdout, sterr
	  //<<<<<<< HEAD
	  cmdfds stdioe = {atoi(optarg), atoi(argv[optind]),
				  atoi(argv[optind+1])};
	  if( (stdioe.fd1 >= fileIndex) || (stdioe.fd2 >= fileIndex) || 
	      (stdioe.fd3 >= fileIndex) )
	    /*=======
	  if( (atoi(optarg) >= fileIndex) || (atoi(argv[optind]) >= fileIndex) || 
	      (atoi(argv[optind+1]) >= fileIndex) )
>>>>>>> c8c7bde80e5d7e88476de1472eabeef227352360
	    */{
	    fprintf( stderr, "Error: File descriptors out of range!\n" );
	    exit(EXIT_FAILURE);
	  }
	  runCommand(stdioe, NULL, NULL);

	  break;

        case '?':
          /* getopt_long already printed an error message. */
        default:
          exit(EXIT_FAILURE);
        }
    }
  /* Print any remaining command line arguments (not options). */
  /*  if (optind < argc)
    {
      printf ("non-option ARGV-elements: ");
      while (optind < argc)
        printf ("%s ", argv[optind++]);
      putchar ('\n');
      }*/

  /* Close file descriptors and free allocated memory */
  for( int i = fileIndex-1; i >= 0; i-- )
    close(files[i]);
  free(files);

  exit (EXIT_SUCCESS);
}

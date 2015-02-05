#include "type.h"
#include "ucode.c"

#define N_BUF 1024

char* pattern, buf[N_BUF];

int main(int argc, char* argv[])
{
  char c;
  int i, r, fd;
  int match, pat_len, line_len, size_left;
  STAT s;

  pattern = argv[1];
  pat_len = strlen(pattern);

  if(1 == argc) {
    printf("USAGE: grep pattern [filename]\n");
    exit(-1);
  }

  write(STDERR, "[===== IFM grep =====]\n\r", 24);

  // if available, use filename as input
  if(argc > 2) {
    stat(argv[2], &s);
    if(0 < s.st_size) {
      fd = open(argv[2], O_RDONLY);
      size_left = s.st_size;
    }
    else {
      printf("ERR: %s does not exist\n", argv[2]);
      exit(-1);
    }
  }
  else {
    fd = STDIN;
    size_left = -1; // no size for stdin
  }

  while(-1 != c && 0 != size_left)
  {
    match = 0;
    i = 0;

    do {
      read(fd, &c, 1);
      buf[i] = c;

      i++;
      size_left--;
    } while( (i < (N_BUF-1)) && (-1 != c) && ('\r' != c) && (size_left != 0) );
    buf[i] = 0;
    

    line_len = strlen(buf);

    for(i=0; i<(line_len-pat_len); i++) {
      if(0==strncmp(pattern, buf+i, pat_len)) {
	match = 1;
	break;
      }
    }
    if(match)
      printf("%s", buf);
  }
}

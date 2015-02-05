#include "type.h"
#include "ucode.c"

#define N_BUF 64



int regmode;

void cput(char c, int fd)
{
  if('\r' == c) {
    write(fd, &c, 1);//write(fd, "\n\r", 2);
  } else {
    write(fd, &c, 1);
    //putc(c);
  }
}

int main(int argc, char* argv[])
{
  int fd, size, bytesLeft, readbytes;
  int i, j;
  char c, buf[N_BUF];
  STAT s;

  char cr = '\r';
  
  stat(STDOUT, &s);

  regmode = 0;
  if( (s.st_mode & 0100000) == 0100000) {// writing to reg file
    regmode = 1;
    write(STDERR, "reg mode\n\r", 10);
  }

  
  write(STDERR, "[===== IFM cat =====]\n\r", 23);

  // read from file
  if(argc > 1)
  {
    fd = open(argv[1], O_RDONLY);
    stat(argv[1], &s);
    bytesLeft = size = s.st_size;

    for(i=0; i<size; i++)
    {   
      readbytes = bytesLeft > N_BUF ? N_BUF : bytesLeft;
      
      read(fd, buf, readbytes);
      for(j=0; j<readbytes; j++)
      {
	c = buf[j];
	if('\n' == c) {
	  putc('\n'); write(STDERR, &cr, 1); //putc('\r');
	} else
	  putc(c);
	
      } // end for(j)

      bytesLeft -= readbytes;
    } // end for(i)

    close(fd);
    exit(0);
  } // end if(argc > 1)

  // read from stdin
  else {
    c = getc();
    while(c != '~' && 0 <= c) // EOT // 04 // -1
    { 
      cput(c, STDOUT);
      if(regmode) // if output to file, echo to screen as well
	cput(c, STDERR);

      c = getc();
    }
    
    putc('\n'); write(STDERR, &cr, 1);
    exit(0);
  } // end else
}



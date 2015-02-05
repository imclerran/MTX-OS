#include "type.h"
#include "ucode.c"

char login[64], passwd[64], *usr, *pw, *fname, *home, *run;
int gid, uid;
char buf[1024];

char *tty;
int stdin, stdout, stderr;

int check_entry(char* line)
{
  usr = strtok(line, ":");
  pw = strtok(0, ":");
  if(0==strcmp(login, usr) && 0==strcmp(passwd, pw)) {
    gid = atoi(strtok(0, ":"));
    uid = atoi(strtok(0, ":"));
    fname = strtok(0, ":");
    home = strtok(0, ":");
    run = strtok(0, ":");
    return 1;
  }
  return 0;
}

int main(int argc, char* argv[]) 
{
  char entry[1024];
  int fd, bytesLeft;
  int i;
  STAT s;

  tty = argv[1];
  close(0);
  close(1);
  close(2);

  stdin = open(tty, 0);
  stdout = open(tty, 1);
  stderr = open(tty, 2);

  while(1) {
    fd = open("/etc/passwd", O_RDONLY);

    printf("IFMlogin : ");
    gets(login);
    printf("password : ");
    gets(passwd);
    
    stat("/etc/passwd", &s);
    bytesLeft = s.st_size;

    while(bytesLeft > 0)
    {
      for(i=0; i<1024 && bytesLeft > 0; i++) 
      {
	read(fd, buf+i, 1);
	bytesLeft--;
	if(buf[i] == '\n') {
	  buf[i] = '\0';
	  break;
	}
      }
      strcpy(entry, buf);

      // check if entry is a match
      if(check_entry(entry)) {
	chdir(home);
	chuid(uid, gid);

	printf("\n  Welcome, %s. Proceed with caution...\n\n", fname);
	exec(run);
      }
    } // end while(bytesLeft)
    close(fd);
    printf("Invalid user/passwd...\n");
  } // end while(1)
}

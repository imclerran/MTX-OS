#include "type.h"
#include "ucode.c"

int pid, status, child[3];
int stdin, stdout;

int main(int argc, char* argv[]) 
{
  int i;
  stdin  = open("/dev/tty0", 0);
  stdout = open("/dev/tty0", 1);

  printf("IFMinit: fork a login task on console...\n");
  
  for(i=0; i<3; i++) {
    child[i] = fork();
    if(!child[i]) // child
      login(i);
  }
  parent();
}

int login(int i)
{
  switch (i) {
    case 0:
	exec("login /dev/tty0");
	printf("login on tty0 failed\n");
	exit(-1);
    case 1:
  printf("ttyS0\n");
	exec("login /dev/ttyS0");
	printf("login on ttyS0 failed\n");
	exit(-1);
    case 2:
	exec("login /dev/ttyS1");
	printf("login on ttyS1 failed\n");
	exit(-1);
  }
}

int get_tty(int pid) {
  if(pid == child[0])
    return 0;
  if(pid == child[1])
    return 1;
  if(pid == child[2])
    return 2;

  return -1;
}

int parent()
{
  int tty;
  while(1)
  { 
    pid = wait(&status); 
    tty = get_tty(pid);

    if(-1 == tty)
      printf("IFMinit: buried an orphan child: pid=%d\n", pid);

    else {
      printf("IFMinit: fork a login task on console...\n");
      child[tty] = fork();
      if(!child[tty])
	login(tty);
    }	
  }
}

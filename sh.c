#include "type.h"
#include "ucode.c"

#define N_BUF 128

char cmdLine[N_BUF];
char* sh_cmd[]={"?", "help", "pwd", "cd", "logout", 0};
char* cmdToks[20];

void menu()
{
  printf("[============== MENU ==============]\n");
  printf("[   ?   help   pwd   cd   logout   ]\n");
  printf("[  cat  grep                       ]\n");
  printf("[============= FSHELL =============]\n");
}


int find_shcmd(char* name)
{
  int i=0;
  char* p = sh_cmd[0];

  while(p) {
    if(0 == strcmp(p, name))
      return i;
    
    i++;
    p = sh_cmd[i];
  }
  return -1;
}

void do_shcmd(int cmd)
{
  char buf[N_BUF];
  switch(cmd)
  {
    case 0:
    case 1: menu(); break;
    case 2: getcwd(buf); printf("%s\n", buf); break;
    case 3: chdir(cmdToks[1]); break;
    case 4: exit(0);
  }
}

void tok_cmd(char* line) //sh_token
{
  int i = 1;
  cmdToks[0] = strtok(line, " ");
  
  while(cmdToks[i-1] != 0) {
    cmdToks[i++] = strtok(0, " ");
    //i++;
  }
}

void strnclr(char* str, int n)
{
  int i;
  for(i=0; i<n; i++)
    *(str+i) = 0;
}


int main(int argc, char* argv[])
{
  int pid, cid, me, status, mode;
  int pd[2];
  int i, j, r;
  char tmp[N_BUF];
  char* cp;
  STAT s;

  me = getpid();

  while(1) {
    printf("FSHELL command ?  ");
    gets(cmdLine);
    if(0 == cmdLine[0])
      continue;

    strcpy(tmp, cmdLine);
    tok_cmd(tmp);
    strnclr(cmdLine, 64);

    // check if shell commmand, if so, execute, and continue
    r = find_shcmd(cmdToks[0]);
    if(r >= 0) {
      do_shcmd(r);
      continue;
    }

    pid = fork();

    if(0==pid) { // child
      cp = cmdLine;

      for(i=0; 0 !=cmdToks[i]; i++) {
        switch(cmdToks[i][0]) {
          
           case '>':
	      // >> append
	      if('>' == cmdToks[i][1]) {
		mode = O_APPEND;
		
	      }
	      // > overwrite
	      else {
		mode = O_WRONLY;
		creat(cmdToks[i+1]);
	      }
 
              close(STDOUT);
              if(open(cmdToks[++i], mode) != STDOUT) {
                write(STDERR, "could not open file\n\r", 21);
                exit(0);
              }
              break;
	  // end case >

          case '<':
	      stat(cmdToks[i+1], &s);
	      if(!s.st_size) {
		printf("%s does not exist\n");
		exit(0);
	      }
	      close(STDIN);
	      open(cmdToks[++i], O_RDONLY);
              break;
	  // end case <

          case '|': // pipe output to program
              pipe(pd);
              pid = fork();
              if(pid) { // parent
		cmdToks[i+1] = '\0';
		close(pd[0]);
		close(STDOUT);
		dup2(pd[1], STDOUT);
              } 
              else { // child
                strnclr(cmdLine, 64);
		cp = cmdLine;
		close(pd[1]);
		close(STDIN);
		dup2(pd[0], STDIN);
              }
              break;
	  // end case |

	  default: // copy args to cmdLine
	      strcpy(cp, cmdToks[i]);
	      cp += strlen(cmdToks[i]);
	      *(cp++) = ' ';
        } // end switch
      } // end for

      exec(cmdLine);
      printf("exec() failed\n");
      exit(1);

    } // end if(0==pid)
    
    else { // parent
      pid = wait(&status);
    }

  } // end while(1)
}


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/inotify.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include "Queue.h"
#include "myfunctions.h"

#define READ 0
#define WRITE 1
#define BUFSIZE 1
#define MAX 50

int flagchld = 0;           //global flag for handler
int flagint = 0;

void chldhandler(int signum){
    signal(SIGCHLD,chldhandler);
    flagchld = 1;
}

void inthandler(int signum){
    signal(SIGINT,inthandler);
    flagint = 1;
}

int main(int argc, char* argv[]){
    pid_t pidl,pidll;
    int p[2];
    char buf[BUFSIZE];
    const char *s = " ";
    char* pathlistener = argv[2];      //take path name

    signal(SIGCHLD,chldhandler);
    signal(SIGINT,inthandler);

    if(pipe(p) == -1){          //create pipe for listener and manager
        perror("Pipe");
        exit(1);
    }

    if((pidll = fork())==-1){     //create listener
        perror("fork");
        exit(1);
    }

    else if (pidll == 0){       //child process Listener

        dup2(p[WRITE],1);   // redirect stdout to pipe
        close(p[READ]);

        //./listenerdir/
        if(argc==1){        //no path given - monitor current directory
            pathlistener = ".";
        }

        char *args[]={"inotifywait",pathlistener,"-m","-e","create","-e","moved_to",NULL};
        execvp(args[0],args);
        perror("execvp in listener");

    }
    else{                       //manager - parent process

		dup2(p[READ],0);   // get stdin from pipe
        close(p[WRITE]);

        queue* q = NULL;
		int i = 0;

        while(1){

			int flag = 0;
			char str[MAX]; 
			char* filepath,*newstr;

            while(read(p[READ],buf,BUFSIZE)>0&& errno!=EINTR){ //read byte per byte the pipe
            
                if(*buf == '\n'){

                    newstr = strndup(str,i);

                    filepath = path(newstr,s);
					
					flag = 1;
					
					printf("Manager read %s from listener\n",filepath);
					
                    break;
                }

                str[i] = *buf;  //store each character into str
                i++;
            }

            if(flagint){            //cltr-c signal to interrupt
                while(q!=NULL){
                    int temppid = dequeue(&q);
                    kill(temppid,SIGKILL);      //terminate all stopped workers
                }

                kill(pidll,SIGKILL);    //terminate listener

                return 0;
            }
			
			if (flagchld){      //received SIGCHLD
                int rpid,status;
                flagchld = 0;
                while((rpid=waitpid(-1,&status,WNOHANG|WUNTRACED))>0){      //receive all stopped children
                    enqueue(&q,rpid);
                }
                print(&q);
            }
			
			if(flag){					//ready filepath
				char pidstring[MAX];
				i = 0;

				if(q == NULL){                //if there are no workers waiting for tasks

					if((pidl = fork())==-1){     //create workers
						perror("fork");
						exit(1);
					}

					if(pidl != 0){              //manager

						char fifo[30];
						fifopath(pidstring,pidl,fifo);

						if ( mkfifo ( fifo , 0666) == -1 ) {   //create named pipe
							if ( errno != EEXIST ) {
								perror ( "receiver : mkfifo " ) ;
								exit(6);
							}
						}

						int np;
						if ( ( np = open ( fifo , O_WRONLY) ) < 0) { //open named pipe
							perror ( " fifo open problem " ) ;
							exit(3);
						}
						write(np, filepath , strlen(filepath)+1);
						close(np);
					}
					else{                       //worker 

						printf("%d process has been created\n",getpid());

						char fifo[MAX]; 
						int length =strlen(filepath)+1;
						char size[MAX];
						sprintf(size,"%d",MAX);          //convert filepath's length to string
						sprintf(pidstring,"%d",getpid());
						fifopath(pidstring,getpid(),fifo);

						char *args[]={"./worker",fifo,size,NULL};
						execvp(args[0],args);
						perror("execvp in worker");
					}
				}
				else{                           //there are available workers into the queue

					pid_t takepid = dequeue(&q);

					printf("Received %d worker pid from queue to continue\n",takepid);
					
					char fifo[MAX];

					sprintf(pidstring,"%d",takepid);
					fifopath(pidstring,takepid,fifo);

					kill(takepid,SIGCONT);              //continue worker

					int np;
					if ( ( np = open ( fifo , O_WRONLY) ) < 0) { //open named pipe
						perror ( " fifo open problem " ) ;
						exit (3) ;
					}
					write(np, filepath , strlen(filepath)+1);
					close(np);
				}
			}
        }
    }
}

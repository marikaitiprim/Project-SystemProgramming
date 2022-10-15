#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include "myfunctions.h"

int workerflag = 0;

void workerhandler(int signum){
    signal(SIGINT,workerhandler);
    workerflag = 1;
}

int main(int argc, char* argv[]){

    const char* protocol = "http://";
    const char* space = " ";
    const char* w = "www.";
    const char* slash = "/";

    signal(SIGINT,workerhandler);

    while(1){
        int filedesc, newfiledesc;
        int size = atoi(argv[2]);
        char filename[size];
        
        int np = open(argv[1], O_RDONLY);
        read(np, filename, sizeof(filename));
        close(np);

        printf("Worker with pid %d takes file %s\n",getpid(),filename);

        if (( filedesc = open(filename , O_RDONLY , 0644 ) ) == -1) { //open file
            perror ( "Opening file" ) ;
            exit(1) ;
        }

        char newfilename[255];
        char* name = onlyfilename(filename,slash);
        outfile(name,newfilename);

        if (( newfiledesc = creat(newfilename, 0644)) == -1) { //create .out file 
            perror("Create file");
            exit(1);
        }            

        struct stat st;
        stat(filename, &st);
        st.st_size = 4*st.st_size;
        char* buffer = malloc(st.st_size*sizeof(char));
        read(filedesc,buffer,st.st_size);
        findProtocol(st.st_size,buffer,space, protocol,w,slash,newfilename);

        if (close(filedesc)  == -1) { //close file
            perror ( "Closing file" ) ;
            exit(1) ;
        }

        free(buffer);

        if(workerflag){
            return 0;
		}

        raise(SIGSTOP);
    }
}

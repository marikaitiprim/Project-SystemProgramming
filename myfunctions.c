#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

typedef struct ListNode {
    char* url;              //adress
    int counter;            //times found into the file             
    struct ListNode* next;
}list;

void push(list** l, char* url){         //insert a node at the end of a list
    
    list* new = malloc(sizeof(list));   //create a new node
    list *last = *l;  
  
    new->url = url;
    new->counter  = 1;
 
    new->next = NULL;
 
    if (*l == NULL){        //list l is empty
       *l = new;
       return;
    }     

    while (last->next != NULL)
        last = last->next;      //find last node
  
    last->next = new;
}

int search(list** l, char* url){
    list* temp = *l;
    while (temp != NULL) {
        if(strcmp(temp->url,url)==0){       //url adress already exists
            temp->counter++;
            return 0;
        }
        temp = temp->next;
    }
    return 1;
}


char* path(char* str, const char* s){

    char* mes1 = strtok(str,s);       //directory
    char* mes2 = mes1;

    for(int i=0; i<2; i++){
        mes2 = strtok(NULL, s);     //ignore event, receive filename
    }
    
    char* mes = strcat(mes1,mes2);  //combine directory with filename

    return mes;
}

char* onlyfilename(char* str,const char* s){        //seperate name from path
    char* mes1 = strtok(str,s);       
    char* mes = mes1;
    char* last;

    while(mes!=NULL){
        last = mes;
        mes = strtok(NULL, s);     
    }

    return last;

}

void outfile(char* filename, char* newfile){

    strcpy( newfile, "./workerdir/" );      //empty directory to put all the .out files
    strcat( newfile, filename );
    strcat( newfile, ".out" );

}

void fifopath(char* str,int pid,char* fifo){

    strcpy(fifo,"./fifos/");                //empty directory to put all the named pipes
    sprintf(str,"%d",pid);          //convert pid to string

    strcat(fifo,str);               //path to folder with named pipes
  
}

char* splitProtocol(char* phrase, const char* protocol, const char* w, const char* slash){
    char* url;
    char* nowww;
    char* sl;
    int adresize,index;
    if(( url = strstr(phrase,protocol))!=NULL){          //http:// exists into this phrase

        if( (nowww = strstr(phrase,w))!=NULL ){             // www exists into this url
            adresize = strlen(nowww)-strlen(w)+1;
            index = strlen(w);
            char* adress = malloc(adresize*sizeof(char));
            strncpy(adress,&nowww[index],adresize);

            if(strstr(adress,slash)!=NULL ){          //there are paths into the adress
                adress = strtok(adress,slash);
            }

            return adress;
        }
        else{
            adresize = strlen(url)-strlen(protocol)+1;
            index = strlen(protocol);
            char* adress = malloc(adresize*sizeof(char));
            strncpy(adress,&url[index],adresize);

            if(strstr(adress,slash)!=NULL ){          //there are paths into the adress
                adress = strtok(adress,slash);
            }

            return adress;
        }

    }

}

void storeProtocol(char* adress,list** l){      //store each url and its counter to the list
    if(search(l,adress)){
        push(l,adress);
    }
}

void writeProtocols(list* l,char* filename){
    int filedesc;

    if (( filedesc = open(filename , O_RDWR , 0644 ) ) == -1) { //open file
        perror ( "Opening file" ) ;
        exit(1);
    }

    while (l != NULL) {     //take each adress from list
        char* strtimes;
        char* line = l->url;
        sprintf(strtimes," %d",l->counter);            //convert to string
        strcat(strtimes,"\n");
        strcat(line,strtimes);      //join adress with counter

        write(filedesc,line,strlen(line));  //write to file
        l = l->next;
    }

    if ( close(filedesc)  == -1) { //close file
        perror ( "Closing file" ) ;
        exit(1) ;
    }
    
}

void findProtocol(int BUFSIZE,char* buffer, const char* space, const char* protocol, const char* w, const char* slash, char* newfilename){

    char* token;
    char* ptr = buffer;

    list* protocols = NULL;
 
    while ((token = strtok_r(ptr, space , &ptr))){    //splitting buffer with spaces
        char* adress = splitProtocol(token,protocol,w, slash);
        if (adress!=NULL && strcmp(adress," ")!=0){
            storeProtocol(adress,&protocols);
        }
    }

    writeProtocols(protocols,newfilename);
}

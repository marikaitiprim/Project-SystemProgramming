#include <stdio.h>
#include <stdlib.h>
#include "Queue.h"
 

queue* new(int k){          //create a queue
    queue* q = malloc(sizeof(queue));
    q->key = k;
    q->next = NULL;
    return q;
}

void print(queue** q){
    queue* temp = *q;
    printf("Queue: ");
    while(temp!=NULL){
        printf("%d ",temp->key);
        temp = temp->next;
    }
    printf("\n");
}


void enqueue(queue** q, int k){   //insert node in the front
    if (*q == NULL) {            //queue is empty
        *q = new(k);            //create a new node
        return;
    }

    queue* temp = new(k);
 
    temp->next = *q;
    *q = temp;
}
 

int dequeue(queue** q)
{
    if (*q == NULL)  //queue is empty
        return -1;

    queue* temp;
    int k;

    if((*q)->next == NULL){ //queue has 1 node
        k = (*q)->key;
        *q = NULL;
    }
    else{                       //queue has more nodes
        temp = (*q);
 
        while(temp->next->next!=NULL){
            temp = temp->next;      //show to the pre- last node
        }

        queue* temp2 = temp->next; //show to the last node
        k = temp2->key;
        temp->next = NULL;
        free(temp2);
    }

    return k;
}


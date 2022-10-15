typedef struct queueNode {
    int key;                
    struct queueNode* next;
}queue;
 
queue* new(int );
void print(queue** );
void enqueue(queue** , int );
int dequeue(queue** );
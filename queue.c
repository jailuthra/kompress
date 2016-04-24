#include <stdlib.h>
#include "queue.h"

struct queue * initqueue()
{
    struct queue *q = malloc(sizeof(struct queue));
    q->front = q->rear = NULL;
}

void freequeue(struct queue *q)
{
    struct queuenode *temp;
    while (temp = front(q)) {
        dequeue(q);
        free(temp);
    }
    free(q);
}

void enqueue(struct queue *q, void *data)
{
    struct queuenode *temp = malloc(sizeof(struct queuenode));
    temp->data = data;
    temp->next = NULL;
    if (q->front == NULL && q->rear == NULL) {
        q->front = q->rear = temp;
        return;
    }
    q->rear->next = temp;
    q->rear = temp;
}

void dequeue(struct queue *q)
{
    struct queuenode *temp = q->front;
    if (q->front == NULL) {
        return;
    }
    if (q->front == q->rear) {
        q->front = q->rear = NULL;
    } else {
        q->front = q->front->next;
    }
    free(temp);
}

void * front(struct queue *q)
{
    if(q->front == NULL) {
        return NULL;
    }
    return q->front->data;
}

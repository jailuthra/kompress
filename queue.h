#pragma once

struct queuenode {
    void *data;
    struct queuenode *next;
};

struct queue {
    struct queuenode *front;
    struct queuenode *rear;
    int size;
};

struct queue * initqueue();
void freequeue(struct queue *q);
void enqueue(struct queue *q, void *data);
void dequeue(struct queue *q);
void * front(struct queue *q);

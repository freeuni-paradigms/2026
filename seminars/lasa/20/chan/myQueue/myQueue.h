/* 
 * A simple queue to help me with the bonus assignment.
 */
#ifndef _QUEUE_H_
#define _QUEUE_H_

typedef struct Node {
    void *elem;
    struct Node *next;
} Node;

typedef struct {
    int elemSize;
    int size;
    Node *head;
    Node *tail;
} Queue;


void queueInit(Queue *queue, int elemSize);
void enQueue(Queue *queue, void *elem);
void deQueue(Queue *queue, void *elem);
void queueDestroy(Queue *queue);

#endif
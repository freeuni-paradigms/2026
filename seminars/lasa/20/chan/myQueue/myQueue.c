#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "myQueue.h"


Node* nodeInit(void *elem, Node *next, int elemSize);

void queueInit(Queue *queue, int elemSize) {
    assert(queue);  
    assert(elemSize > 0);
    
    queue->size = 0;
    queue->elemSize = elemSize;
    queue->head = NULL;
    queue->tail = NULL;
}

void enQueue(Queue *queue, void *elem) {
    Node *newNode = nodeInit(elem, NULL, queue->elemSize);
    assert(newNode);

    if (queue->size == 0)
        queue->head = newNode;
    
    if (queue->tail != NULL) 
        queue->tail->next = newNode;
    queue->tail = newNode;

    queue->size++;
}

Node* nodeInit(void* elem, Node *next, int elemSize) {
    Node *node = malloc(sizeof(Node));
    node->elem = malloc(elemSize);
    memcpy(node->elem, elem, elemSize);
    node->next = next;
    return node;
}

void deQueue(Queue *queue, void *elem) {
    assert(queue->size);
    assert(elem != NULL);

    Node *temp = queue->head->next;
    Node *ret = queue->head;
    if (queue->head == queue->tail) queue->tail = temp;
    queue->head = temp;
    queue->size--;

    memcpy(elem, ret->elem, queue->elemSize);
    free(ret->elem);
    free(ret);
}

void queueDestroy(Queue *queue) {
    if (queue->size == 0) return;

    while (queue->size != 0) {
        Node *temp = queue->head->next;
        free(queue->head->elem);
        free(queue->head);
        queue->head = temp;
        queue->size--;
    }
}
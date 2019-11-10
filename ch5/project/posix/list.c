/**
 * Various list operations
 */
 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "list.h"
#include "task.h"


// add a new task to the list of tasks
void insert(struct node **head, Task *newTask) {
    // add the new task to the list 
    struct node *newNode = malloc(sizeof(struct node));

    newNode->task = newTask;
    newNode->next = *head;
    *head = newNode;
}

// delete the selected task from the list
void delete(struct node **head, Task *task) {
    struct node *temp;
    struct node *prev;

    temp = *head;
    // special case - beginning of list
    if (strcmp(task->name,temp->task->name) == 0) {
        *head = (*head)->next;
    }
    else {
        // interior or last element in the list
        prev = *head;
        temp = temp->next;
        while (strcmp(task->name,temp->task->name) != 0) {
            prev = temp;
            temp = temp->next;
        }

        prev->next = temp->next;
    }
}

// traverse the list
void traverse(struct node *head) {
    struct node *temp;
    temp = head;

    while (temp != NULL) {
        printf("[%s] [%d] [%d]\n",temp->task->name, temp->task->priority, temp->task->burst);
        temp = temp->next;
    }
}

// reverse the list
void reverse(struct node **head) {
    struct node *next;
    struct node *temp;
    
    if (!(*head) || !(*head)->next)
        return;
    temp = (*head)->next;
    (*head)->next = NULL;
    while (temp) {
        next = temp->next;
        temp->next = *head;
        *head = temp;
        temp = next;
    }
}

// sort the list (simple insertion sort)
void sort(struct node **head, int (*cmp)(Task *, Task *)) {
    struct node *temp;
    struct node *next;
    struct node *curr;

    if (!(*head) || !(*head)->next)
        return;
    curr = (*head)->next;
    (*head)->next = NULL;
    while (curr) {
        next = curr->next;
        curr->next = NULL;
        if (cmp(curr->task, (*head)->task) <= 0) {
            curr->next = *head;
            *head = curr;
            curr = next;
            continue;
        }
        temp = *head;
        while (temp->next) {
            if(cmp(curr->task, temp->next->task) <= 0) {
                curr->next = temp->next;
                temp->next = curr;
                break;
            }
            temp = temp->next;
        }
        if (!temp->next) {
            temp->next = curr;
        }
        curr = next;
    }
}

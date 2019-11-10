#include <stdlib.h>
#include "task.h"
#include "cpu.h"
#include "list.h"
#include "schedulers.h"


static struct node *head = NULL;

// add a task to the list 
void add(char *name, int priority, int burst) {
    Task *tsk = (Task *)malloc(sizeof(Task));
    tsk->name = name;
    tsk->priority = priority;
    tsk->burst = burst;
    insert(&head, tsk);
}


// invoke the scheduler
void schedule() {
    struct node *temp;
    struct node *curr;
    struct node *next;
    reverse(&head);
    curr = head;
    while(curr) {
        next = curr->next;
        curr->next = NULL;
        if (!next || curr->task->burst <= QUANTUM) { 
            run(curr->task, curr->task->burst);
            free(curr->task);
            free(curr);
        }
        else {
            run(curr->task, QUANTUM);
            curr->task->burst -= QUANTUM;
            temp = next;
            while(temp->next)
                temp = temp->next;
            temp->next = curr;
        }
        curr = next;
    }   
}

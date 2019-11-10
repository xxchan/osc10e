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
    struct node *next;
    reverse(&head);
    temp = head;
    while(temp) {
        run(temp->task, temp->task->burst);
        next = temp->next;
        free(temp->task);
        free(temp);
        temp = next;
    }
}

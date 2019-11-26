/** 
 * simple contiguous memory allocator
 * implicit list implementation
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* p points to a free block of at least size bytes */
#define IS_VALID(p, size) (p->pid == -1 && p->limit >= size)

typedef int pid_t;
/* memory block */
typedef struct block
{
    size_t base;
    size_t limit;
    struct block *next;
    /* pid = -1 means free block */
    pid_t pid;
} block_t;

static block_t *head = NULL;

void init(size_t init_size);
void free_list();

int request(pid_t pid, size_t size, char fit_mode);
int release(pid_t pid);
void compact();
void report_status();

int main(int argc, char *argv[])
{
    char *line = NULL;
    char *word;
    size_t line_size = 0;
    size_t init_size = atoi(argv[1]);
    size_t req_size;
    pid_t pid;
    char mode;
    init(init_size);
    while (1)
    {
        printf("allocator>");
        fflush(stdout);
        getline(&line, &line_size, stdin);
        if (strcmp(line, "C") == 0)
        {
            compact();
        }
        else if (strncmp(line, "STAT", 4) == 0)
        {
            report_status();
        }
        else if (strncmp(line, "RQ", 2) == 0)
        {
            word = strtok(line, " \t\r\n");
            word = strtok(NULL, " \t\r\n");
            pid = atoi(word + 1);
            word = strtok(NULL, " \t\r\n");
            req_size = atoi(word);
            word = strtok(NULL, " \t\r\n");
            mode = *word;
            if (mode != 'F' && mode != 'B' && mode != 'W')
            {
                printf("illegal fit mode\n");
                continue;
            }
            if (request(pid, req_size, mode) != 0)
                printf("request denied\n");
        }
        else if (strncmp(line, "RL", 2) == 0)
        {
            word = strtok(line, " \t\r\n");
            word = strtok(NULL, " \t\r\n");
            pid = atoi(word + 1);
            release(pid);
        }
        else if (strncmp(line, "X", 1) == 0)
        {
            break;
        }
        else
        {
            printf("invalid command\n");
        }
    }
    free(line);
    free_list();
}

void init(size_t init_size)
{
    head = malloc(sizeof(block_t));
    block_t *node = malloc(sizeof(block_t));
    node->base = 0;
    node->limit = init_size;
    node->pid = -1;
    node->next = NULL;
    head->next = node;
}

void free_list()
{
    block_t *tmp;
    while (head->next)
    {
        tmp = head->next;
        head->next = tmp->next;
        free(tmp);
    }
    free(head);
}

int request(pid_t pid, size_t size, char fit_mode)
{
    block_t *p = head;
    /* target->next is allocated for the request */
    block_t *target = NULL;
    switch (fit_mode)
    {
    case 'F':
        while (p->next)
        {
            if (IS_VALID(p->next, size))
            {
                target = p;
                break;
            }
            p = p->next;
        }
        break;
    case 'B':
        while (p->next)
        {
            if (IS_VALID(p->next, size) && (target == NULL || p->next->limit < target->next->limit))
                target = p;
            p = p->next;
        }
        break;
    case 'W':
        while (p->next)
        {
            if (IS_VALID(p->next, size) && (target == NULL || p->next->limit > target->next->limit))
                target = p;
            p = p->next;
        }
        break;
    }
    if (target == NULL)
    {
        printf("no available memory block\n");
        return -1;
    }
    target->next->pid = pid;
    /* need splitting */
    if (target->next->limit != size)
    {
        block_t *node = malloc(sizeof(block_t));
        node->base = target->next->base + size;
        node->limit = target->next->limit - size;
        node->next = target->next->next;
        node->pid = -1;
        target->next->limit = size;
        target->next->next = node;
    }
    return 0;
}

int release(pid_t pid)
{
    if (pid < 0)
    {
        printf("invalid pid\n");
        return -1;
    }
    block_t *p = head;
    block_t *q;
    while (p->next)
    {
        if (p->next->pid == pid)
        {
            p->next->pid = -1;
            /* forward coalescing */
            if (p->next->next && p->next->next->pid == -1)
            {
                q = p->next->next;
                p->next->limit += q->limit;
                p->next->next = q->next;
                free(q);
            }
            /* backward coalescing */
            if (p->pid == -1)
            {
                q = p->next;
                p->limit += q->limit;
                p->next = q->next;
                free(q);
            }
            return 0;
        }
        p = p->next;
    }
    printf("process %d not found\n", pid);
    return -1;
}

void compact()
{
    block_t *p = head;
    block_t *q;
    size_t offset = 0;
    while (p->next)
    {
        if (p->next->pid == -1)
        {
            /* the last block */
            if (p->next->next == NULL)
            {
                p->next->limit += offset;
                p->next->base -= offset;
            }
            else
            {
                q = p->next;
                offset += q->limit;
                p->next = q->next;
                free(q);
            }
        }
        else
        {
            p->base -= offset;
            p = p->next;
        }
    }
}

void report_status()
{
    block_t *p = head;
    while (p->next)
    {
        printf("Addresses [%lu:%lu] ", p->next->base, p->next->base + p->next->limit - 1);
        if (p->next->pid == -1)
            printf("Unused\n");
        else
            printf("Process P%d\n", p->next->pid);
        p = p->next;
    }
}